#include "rtl_binaryxml.h"

/* construct a TBinaryXml object on the specified buffer
 * attention the construct() function will automatically create the root 
 * node on the buffer. it will occupy 4 bytes. the root node holds the 
 * information of the total length of the xml.
 * 
 * @param
 * 	xml			point to an TBinaryXml varaible not initilized
 * 	xmldata		databuffer to hold the binary xml data. 
 * 				it is often the address point to the payload of the packet.
 * 	size		xml data buffer size
 */
TBinaryXml * xml_construct( TBinaryXml * xml, char * xmldata, uint8 size )
{
	xml->buf = xmldata;
	xml->size = size;
	xml->buf[0] = 4;
	xml->buf[1] = 0;
	xml->buf[2] = 0;//TOTAL_SIZE;
	xml->buf[3] = size;
	xml->length = 0;
	xml->current = 4;
	
	return xml;
}

void xml_destroy( TBinaryXml * xml )
{
	NULL;
}

/* attach the TBinaryXml buffer to an exist buffer. the data in the buffer will 
 * not changed */
void  xml_attach( TBinaryXml * xml, char * xmldata, uint8 len )
{
	xml->buf = xmldata;
	xml->length = 0;	
}

/* allocate a new node in the memory buffer.
 * @return
 * 	0	when failed
 * 	>0	success. it's the identifier of the node, you can manipulate this node
 * 		through this id. actually, the value is essentially the index of the node
 * 		in the buffer. so the implementation of xml_read/xml_write is highly 
 * 		efficient.
 */
uint8 xml_newnode( TBinaryXml * xml, uint8 request_size )
{
	return 0;
} 

/* append a node in the buffer 
 */
uint8 xml_append( TBinaryXml * xml, uint8 parid, uint8 property, char * data, uint8 datalen )
{
	uint8 id = xml_newnode( xml, datalen );
	if (id > 0)
	{
		xml_write( xml, id, property, data, datalen );
	}
	
	return id;
}


/* update a node. the node should be already allocated by xml_newnode() or xml_append()
 * the size of the node will keep unchanged. 
 * 
 * @return
 * the bytes successfully wroten
 */
uint8 xml_write( TBinaryXml * xml, uint8 id, uint8 property, char * data, uint8 datalen )
{
	return 0;
}

/* @return
 * the bytes successfully read
 */
uint8 xml_read( TBinaryXml * xml, uint8 id, uint8 * property, char * data, uint8 size )
{
	return 0;
}

/* remove a node from the xml tree structure
 * not implemented. 
 * i think it is not necessary for such embedded systems. but i still keep this 
 * interface here for future use.
 */
void xml_remove( TBinaryXml * xml, uint8 id )
{
	return;
}

/* find the id of parent node */
uint8 xml_findparent( TBinaryXml * xml, uint8 id )
{
	return 0;
}

/* find the id of the first child */
uint8 xml_findchild( TBinaryXml * xml, uint8 id )
{
	return 0;
}

/* find the id of previous brother node */
uint8 xml_findprev( TBinaryXml * xml, uint8 id )
{
	return 0;
}

/* find the id of the next brother node */
uint8 xml_findnext( TBinaryXml * xml, uint8 id )
{
	return 0;
}

