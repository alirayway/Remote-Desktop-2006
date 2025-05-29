

// rdEncodeZlib

// This file implements the rdEncoder-derived rdEncodeZlib class.
// This class overrides some rdEncoder functions to produce a bitmap
// to Zlib encoder.  Zlib is much more efficient than RAW format on
// most screen data and usually twice as efficient as hextile.  Of
// course, zlib compression uses more CPU time on the server.
// However, over slower (64kbps or less) connections, the reduction
// in data transmitted usually outweighs the extra latency added
// while the server CPU performs the compression algorithms.

#include "rdEncodeZlib.h"

rdEncodeZlib::rdEncodeZlib()
{
	m_buffer = NULL;
	m_bufflen = 0;
	compStreamInited = false;
}

rdEncodeZlib::~rdEncodeZlib()
{
	if (m_buffer != NULL)
	{
		delete [] m_buffer;
		m_buffer = NULL;
	}
	if ( compStreamInited == true )
	{
		deflateEnd( &compStream );
	}
	compStreamInited = false;
}

void
rdEncodeZlib::Init()
{
	rdEncoder::Init();
}

UINT
rdEncodeZlib::RequiredBuffSize(UINT width, UINT height)
{
	int result;

	// The zlib library specifies a maximum compressed size of
	// the raw size plus one percent plus 8 bytes.  We also need
	// to cover the zlib header space.
	result = rdEncoder::RequiredBuffSize(width, height);
	result += ((result / 100) + 8) + sz_rfbZlibHeader;
	return result;
}

UINT
rdEncodeZlib::NumCodedRects(RECT &rect)
{

/******************************************************************
	return 1;
******************************************************************/

	const rectW = rect.right - rect.left;
	const rectH = rect.bottom - rect.top;

	// Return the number of rectangles needed to encode the given
	// update.  ( ZLIB_MAX_SIZE(rectW) / rectW ) is the number of lines in 
	// each maximum size rectangle.
	return (( rectH - 1 ) / ( ZLIB_MAX_SIZE( rectW ) / rectW ) + 1 );

}

/*****************************************************************************
 *
 * Routines to implement zlib Encoding (LZ+Huffman compression) by calling
 * the included zlib library.
 */

// Encode the rectangle using zlib compression
inline UINT
rdEncodeZlib::EncodeRect(BYTE *source, VSocket *outConn, BYTE *dest, const RECT &rect)
{
	int  totalSize = 0;
	int  partialSize = 0;
	int  maxLines;
	int  linesRemaining;
	RECT partialRect;

	const rectW = rect.right - rect.left;
	const rectH = rect.bottom - rect.top;

	partialRect.right = rect.right;
	partialRect.left = rect.left;
	partialRect.top = rect.top;
	partialRect.bottom = rect.bottom;

	/* WBB: For testing purposes only! */
	// rdlog.Print(LL_INTINFO, rdLOG("rect.right=%d rect.left=%d rect.top=%d rect.bottom=%d\n"), rect.right, rect.left, rect.top, rect.bottom);

	maxLines = ( ZLIB_MAX_SIZE(rectW) / rectW );
	linesRemaining = rectH;

	while ( linesRemaining > 0 ) {

		int linesToComp;

		if ( maxLines < linesRemaining )
			linesToComp = maxLines;
		else
			linesToComp = linesRemaining;

		partialRect.bottom = partialRect.top + linesToComp;

		/* WBB: For testing purposes only! */
		// rdlog.Print(LL_INTINFO, rdLOG("partialRect.right=%d partialRect.left=%d partialRect.top=%d partialRect.bottom=%d\n"), partialRect.right, partialRect.left, partialRect.top, partialRect.bottom);

		partialSize = EncodeOneRect( source, dest, partialRect );
		totalSize += partialSize;

		linesRemaining -= linesToComp;
		partialRect.top += linesToComp;

		if (( linesRemaining > 0 ) &&
			( partialSize > 0 ))
		{
			// Send the encoded data
			outConn->SendQueued( (char *)dest, partialSize );
			transmittedSize += partialSize;
		}


	}
	transmittedSize += partialSize;

	return partialSize;

}

// Encode the rectangle using zlib compression
inline UINT
rdEncodeZlib::EncodeOneRect(BYTE *source, BYTE *dest, const RECT &rect)
{
	int totalCompDataLen = 0;
	int previousTotalOut;
	int deflateResult;

	const rectW = rect.right - rect.left;
	const rectH = rect.bottom - rect.top;
	const rawDataSize = (rectW*rectH*m_remoteformat.bitsPerPixel / 8);
	const maxCompSize = (rawDataSize + (rawDataSize/100) + 8);

	// Send as raw if the update is too small to compress.
	if (rawDataSize < rd_ENCODE_ZLIB_MIN_COMP_SIZE)
		return rdEncoder::EncodeRect(source, dest, rect);

	// Create the rectangle header
	rfbFramebufferUpdateRectHeader *surh=(rfbFramebufferUpdateRectHeader *)dest;
	surh->r.x = (CARD16) rect.left;
	surh->r.y = (CARD16) rect.top;
	surh->r.w = (CARD16) (rectW);
	surh->r.h = (CARD16) (rectH);
	surh->r.x = Swap16IfLE(surh->r.x);
	surh->r.y = Swap16IfLE(surh->r.y);
	surh->r.w = Swap16IfLE(surh->r.w);
	surh->r.h = Swap16IfLE(surh->r.h);
	surh->encoding = Swap32IfLE(rfbEncodingZlib);

	dataSize += ( rectW * rectH * m_remoteformat.bitsPerPixel) / 8;
	rectangleOverhead += sz_rfbFramebufferUpdateRectHeader;
	
	// create a space big enough for the Zlib encoded pixels
	if (m_bufflen < rawDataSize)
	{
		if (m_buffer != NULL)
		{
			delete [] m_buffer;
			m_buffer = NULL;
		}
		m_buffer = new BYTE [rawDataSize+1];
		if (m_buffer == NULL)
			return rdEncoder::EncodeRect(source, dest, rect);
		m_bufflen = rawDataSize;
	}

	// Translate the data into our new buffer
	Translate(source, m_buffer, rect);

	// Initialize input/output buffer assignment for compressor state.
	compStream.avail_in = rawDataSize;
	compStream.next_in = m_buffer;
	compStream.avail_out = maxCompSize;
	compStream.next_out = (dest+sz_rfbFramebufferUpdateRectHeader+sz_rfbZlibHeader);
	compStream.data_type = Z_BINARY;

	// If necessary, the first time, initialize the compressor state.
	if ( compStreamInited == false )
	{

		compStream.total_in = 0;
		compStream.total_out = 0;
		compStream.zalloc = Z_NULL;
		compStream.zfree = Z_NULL;
		compStream.opaque = Z_NULL;

		rdlog.Print(LL_INTINFO, rdLOG("calling deflateInit2 with zlib level:%d\n"), m_compresslevel);

		deflateResult = deflateInit2( &compStream,
			                          m_compresslevel,
					                  Z_DEFLATED,
					                  MAX_WBITS,
					                  MAX_MEM_LEVEL,
					                  Z_DEFAULT_STRATEGY );
		if ( deflateResult != Z_OK )
		{
			rdlog.Print(LL_INTINFO, rdLOG("deflateInit2 returned error:%d:%s\n"), deflateResult, compStream.msg);
			return rdEncoder::EncodeRect(source, dest, rect);
		}
		compStreamInited = true;
	}

	// Record previous total output size.
	previousTotalOut = compStream.total_out;

	// Compress the raw data into the result buffer.
	deflateResult = deflate( &compStream, Z_SYNC_FLUSH );

	if ( deflateResult != Z_OK )
	{
		rdlog.Print(LL_INTINFO, rdLOG("deflate returned error:%d:%s\n"), deflateResult, compStream.msg);
		return rdEncoder::EncodeRect(source, dest, rect);
	}

	// Calculate size of compressed data.
	totalCompDataLen = compStream.total_out - previousTotalOut;

	// Format the ZlibHeader
	rfbZlibHeader *zlibh=(rfbZlibHeader *)(dest+sz_rfbFramebufferUpdateRectHeader);
	zlibh->nBytes = Swap32IfLE(totalCompDataLen);

	// Update statistics
	encodedSize += sz_rfbZlibHeader + totalCompDataLen;

	// Return the amount of data sent	
	return sz_rfbFramebufferUpdateRectHeader +
		   sz_rfbZlibHeader +
		   totalCompDataLen;

}
