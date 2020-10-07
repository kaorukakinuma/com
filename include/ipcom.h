/*
 *    file:             ipcom.h
 *    creation date:    2020-10-07
 *    last update:      2020-10-07
 *    author:           kaoru kakinuma
 */
#ifndef _IPCOM_H_
#define _IPCOM_H_

#include "ipcom_types.h"
struct Ipcom {
	IpcomErcd (*Open )( Ipcom *pSelf );
	IpcomErcd (*Close)( Ipcom *pSelf );
	IpcomErcd (*Read )( Ipcom *pSelf, char *pBuffer, int length );
	IpcomErcd (*Write)( Ipcom *pSelf, const char *pBuffer, int length );
};

#endif /* _IPCOM_H_ */
