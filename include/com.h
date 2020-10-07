/*
 *    file:             com.h
 *    creation date:    2020-10-07
 *    last update:      2020-10-07
 *    author:           kaoru kakinuma
 */
#ifndef _COM_H_
#define _COM_H_

#include "com_types.h"
struct Com {
	ComErcd (*Open )( Com *pSelf );
	ComErcd (*Close)( Com *pSelf );
	ComErcd (*Read )( Com *pSelf, char *pBuffer, int length );
	ComErcd (*Write)( Com *pSelf, const char *pBuffer, int length );
};

#endif /* _COM_H_ */
