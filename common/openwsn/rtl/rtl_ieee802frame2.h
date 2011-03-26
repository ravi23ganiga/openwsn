/**
 * IEEE802.2 Logic Link Layer Frame
 *
 * LLC PDU Format
 *  +-----------------+-----------------+------------------+----------------+
 *  | DSAP Address 1B | SSAP Address 1B | Control 1B or 2B | Information nB |
 *  +-----------------+-----------------+------------------+----------------+
 * 
 *  DSAP Address: destination service access point address field
 *  SSAP Address: source service access point address field
 *  Control: 16bits include sequence numbering, and 8bits for formats that do not
 *  Information: information field
 *  
 * reference
 *  IEEE 802.11 specification
 */

