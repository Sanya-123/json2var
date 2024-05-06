/**
 ******************************************************************************
 * @file    conf2var.h
 * @author  kasper
 * @date    2023-Jul-13
 * @brief   Description
 ******************************************************************************
 */
#ifndef INC_CONF2VAR_H_
#define INC_CONF2VAR_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Exported includes ---------------------------------------------------------*/
#include <stdint.h>
#include "varcommon/varcommon.h"
/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

int var2json(varloc_node_t* root_node, char* file_path);
varloc_node_t* json2var(char* file_path);

#ifdef __cplusplus
}
#endif

#endif /* INC_CONF2VAR_H_ */
