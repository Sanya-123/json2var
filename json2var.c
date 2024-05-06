#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"
#include "varcommon/varcommon.h"

static void write_node(varloc_node_t* node, cJSON* json){
    if (node == NULL || json == NULL)
        return;

    if(!cJSON_IsArray(json))
        return;


    cJSON* group = cJSON_CreateObject();

    if(group != NULL)
    {
        cJSON_AddItemToArray(json, group);

        cJSON_AddStringToObject(group, "name", node->name);
        cJSON_AddStringToObject(group, "ctype", node->ctype_name);
        cJSON_AddNumberToObject(group, "nodetype", node->var_type);
        cJSON_AddNumberToObject(group, "address", node->address.base);
        cJSON_AddNumberToObject(group, "size", node->address.size_bits);
        cJSON_AddNumberToObject(group, "offset", node->address.offset_bits);

        cJSON_AddNumberToObject(group, "signed", node->is_signed);
        cJSON_AddNumberToObject(group, "float", node->is_float);

        if (node->child != NULL)
        {
            cJSON* list = cJSON_AddArrayToObject(group, "members");
            write_node(node->child, list);
        }
    }
    else{
        printf("Group null for node: %s\n", node->name);
    }

    if (node->next != NULL){
        write_node(node->next, json);
    }
}

static varloc_node_t* read_node(cJSON* json){
    if (json == NULL){
        return NULL;
    }

    if(!cJSON_IsArray(json))
    {
        return NULL;
    }

    int count = cJSON_GetArraySize(json);
    varloc_node_t* ret = NULL;
    varloc_node_t* node = NULL;
    for(int i = 0; i < count; ++i){
        if (ret != NULL){
            // sibling
            varloc_node_t* sibling = new_var_node();
            node->next = sibling;
            sibling->previous = node;
            node = sibling;
        }
        else{
            // first node on level
            node = new_var_node();
            ret = node;
        }
        cJSON* var = cJSON_GetArrayItem(json, i);
        cJSON* var_name = cJSON_GetObjectItem(var, "name");
        cJSON* var_ctype_name = cJSON_GetObjectItem(var, "ctype");
        char* name = cJSON_GetStringValue(var_name);
        char* ctype_name = cJSON_GetStringValue(var_ctype_name);
        if(name != NULL){
            strcpy(node->name, name);
        }
        if(ctype_name != NULL){
            strcpy(node->ctype_name, ctype_name);
        }
//        int sign = 0;
        node->var_type = (varloc_node_type_t)(int)cJSON_GetNumberValue(cJSON_GetObjectItem(var, "nodetype"));
        node->address.base = (uint32_t)cJSON_GetNumberValue(cJSON_GetObjectItem(var, "address"));
        node->address.size_bits = (uint32_t)cJSON_GetNumberValue(cJSON_GetObjectItem(var, "size"));
        node->address.offset_bits = (uint32_t)cJSON_GetNumberValue(cJSON_GetObjectItem(var, "offset"));
        node->is_signed = (int)cJSON_GetNumberValue(cJSON_GetObjectItem(var, "signed"));
        node->is_float = (int)cJSON_GetNumberValue(cJSON_GetObjectItem(var, "float"));

//        if (sign == VARLOC_SIGNED){node->is_signed = 1;}
//        if (sign == VARLOC_FLOAT){node->is_float = 1;}

        cJSON* members = cJSON_GetObjectItem(var, "members");
        if(members != NULL){
            node->child = read_node(members);
            if (node->child){
                node->child->parent = node;
            }
        }
    }
    return ret;
}

static char* read_file(const char *filename) {
    FILE *file = NULL;
    long length = 0;
    char *content = NULL;
    size_t read_chars = 0;

    /* open in read binary mode */
    file = fopen(filename, "rb");
    if (file == NULL)
    {
        goto cleanup;
    }

    /* get the length */
    if (fseek(file, 0, SEEK_END) != 0)
    {
        goto cleanup;
    }
    length = ftell(file);
    if (length < 0)
    {
        goto cleanup;
    }
    if (fseek(file, 0, SEEK_SET) != 0)
    {
        goto cleanup;
    }

    /* allocate content buffer */
    content = (char*)malloc((size_t)length + sizeof(""));
    if (content == NULL)
    {
        goto cleanup;
    }

    /* read the file into memory */
    read_chars = fread(content, sizeof(char), (size_t)length, file);
    if ((long)read_chars != length)
    {
        free(content);
        content = NULL;
        goto cleanup;
    }
    content[read_chars] = '\0';


cleanup:
    if (file != NULL)
    {
        fclose(file);
    }

    return content;
}

static int write_file(cJSON *json, const char *filename) {
    FILE *file = NULL;
    int ret = -EXIT_FAILURE;

    /* open in read binary mode */
    file = fopen(filename, "wb");
    if (file == NULL)
    {
        goto cleanup;
    }

    /* write to file */
    char *saveData = cJSON_Print(json);
    ret = fwrite(saveData, sizeof(char), strlen(saveData), file);


cleanup:
    if (file != NULL)
    {
        fclose(file);
    }

    return ret;
}

int var2json(varloc_node_t* root_node, char* file_path){
    cJSON *json = /*cJSON_CreateObject()*/cJSON_CreateObject();
    int ret = -1;
    if (json == NULL)
    {
        return -2;
    }

    cJSON *variables_array = cJSON_AddArrayToObject(json, "variables");
    if (variables_array == NULL)
    {
        ret = -3;
        goto cleanup;
    }

    write_node(root_node, variables_array);

    /* Write out the new configuration. */
    ret = write_file(json, file_path);

cleanup:

    cJSON_Delete(json);

    return ret;
}


varloc_node_t* json2var(char* file_path){

    char *file = NULL;
    cJSON *json = NULL;

    file = read_file(file_path);
    if(file == NULL)
        return NULL;

    json = cJSON_Parse(file);
    free(file);

    if(json == NULL)
        return NULL;

    varloc_node_t* ret = read_node(cJSON_GetObjectItem(json, "variables"));

    cJSON_Delete(json);


    return ret;
}

