/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
#include <tree_sitter/api.h>
#include <string.h>
#include "subtree.h"

/* Header for class jsitter_interop_JSitter */
    /*
     * Class:     jsitter_interop_JSitter
     * Method:    getSymbolName
     * Signature: (JS)Ljava/lang/String;
     */
    JNIEXPORT jstring JNICALL Java_jsitter_interop_JSitter_getSymbolName
    (JNIEnv *env, jclass cl, jlong language_ptr, jint ts_symbol) {
        const char *name = ts_language_symbol_name((TSLanguage *)language_ptr, (TSSymbol)ts_symbol);
        return (*env)->NewStringUTF(env, name);
    }
    
    /*
     * Class:     jsitter_interop_JSitter
     * Method:    isTerminal
     * Signature: (JS)Z
     */
    JNIEXPORT jboolean JNICALL Java_jsitter_interop_JSitter_isTerminal
    (JNIEnv *env, jclass class, jlong language_ptr, jint ts_symbol) {
        return ts_language_symbol_type((TSLanguage*)language_ptr, (TSSymbol)ts_symbol) == TSSymbolTypeAnonymous;
    }
    
    JNIEXPORT jboolean JNICALL JavaCritical_jsitter_interop_JSitter_isTerminal
    (jlong language_ptr, jint ts_symbol) {
        return ts_language_symbol_type((TSLanguage*)language_ptr, (TSSymbol)ts_symbol) == TSSymbolTypeAnonymous;
    }
    
    /*
     * Class:     jsitter_interop_JSitter
     * Method:    getSymbolByName
     * Signature: (JLjava/lang/String;)S
     */
    JNIEXPORT jint JNICALL Java_jsitter_interop_JSitter_getSymbolByName
    (JNIEnv *env, jclass class, jlong language_ptr, jstring name) {
        jboolean copy;
        const char *chars = (*env)->GetStringUTFChars(env, name, &copy);
        TSSymbol symbol = ts_language_symbol_for_name((TSLanguage *)language_ptr, chars, strlen(chars), true);
        if (copy) {
            (*env)->ReleaseStringUTFChars(env, name, chars);
        }
        return symbol;
    }
    
    JNIEXPORT void JNICALL Java_jsitter_interop_JSitter_releaseSubtree
    (JNIEnv *env , jclass class, jlong subtree_ptr) {
        SubtreePool pool = ts_subtree_pool_new(0);
        ts_subtree_release(&pool, *(Subtree *)(&subtree_ptr));
        ts_subtree_pool_delete(&pool);
    }
    
    JNIEXPORT void JNICALL Java_jsitter_interop_JSitter_retainSubtree
    (JNIEnv *env , jclass class, jlong subtree_ptr) {
        ts_subtree_retain(*(Subtree *)(&subtree_ptr));
    }
    
    /*
     * Class:     jsitter_interop_JSitter
     * Method:    releaseTree
     * Signature: (J)V
     */
    JNIEXPORT void JNICALL Java_jsitter_interop_JSitter_releaseTree
    (JNIEnv *env , jclass class, jlong tree_ptr) {
        ts_tree_delete((TSTree *)tree_ptr);
    }
    
    /*
     * Class:     jsitter_interop_JSitter
     * Method:    releaseParser
     * Signature: (J)V
     */
    JNIEXPORT void JNICALL Java_jsitter_interop_JSitter_releaseParser
    (JNIEnv *env , jclass class, jlong parser_ptr) {
        ts_parser_delete((TSParser *)parser_ptr);
    }
    
    struct Input {
        JNIEnv *env;
        jobject input;
        jmethodID read_mtd;
        const char *reading_addr;
    };
    
    const char *input_jni_read(void *payload, uint32_t byte_index, TSPoint position, uint32_t *bytes_read) {
        struct Input *input = (struct Input *)payload;
        jint read = (*input->env)->CallIntMethod(input->env, input->input, input->read_mtd, byte_index);
        *bytes_read = read;
        return input->reading_addr;
    }

    /*
     * Class:     jsitter_interop_JSitter
     * Method:    parse
     * Signature: (JJLjsitter/interop/JSitter/Input;JIII)J
     */
    JNIEXPORT jlong JNICALL Java_jsitter_interop_JSitter_parse
    (JNIEnv *env, jclass class, jlong parser_ptr, jlong old_tree_ptr, jobject input, jint encoding, jobject reading_buffer) {
        static jmethodID read_mtd = 0;
        if (read_mtd == 0) {
            jclass input_class = (*env)->FindClass(env, "jsitter/interop/JSitter$Input");
            read_mtd = (*env)->GetMethodID(env, input_class, "read", "(I)I");
        }
        void *reading_addr = (*env)->GetDirectBufferAddress(env, reading_buffer);
        struct Input input_ctx;
        input_ctx.env = env;
        input_ctx.input = input;
        input_ctx.read_mtd = read_mtd;
        input_ctx.reading_addr = (const char *)reading_addr;
        
        TSInput ts_input;
        ts_input.encoding = (TSInputEncoding)encoding;
        ts_input.payload = &input_ctx;
        ts_input.read = &input_jni_read;
        
        TSTree *old_tree = (TSTree *)old_tree_ptr;
        TSParser *parser = (TSParser *)parser_ptr;

        TSTree *new_tree = ts_parser_parse(parser, old_tree, ts_input);
        return (jlong)new_tree;
    }

    JNIEXPORT void JNICALL Java_jsitter_interop_JSitter_editTree
    (JNIEnv *env, jclass class, jlong tree_ptr, jint start_byte, jint old_end_byte, jint new_end_byte) {
        TSInputEdit edit;
        edit.start_byte = start_byte;
        edit.old_end_byte = old_end_byte;
        edit.new_end_byte = new_end_byte;
        ts_tree_edit((TSTree *)tree_ptr, &edit);
     }

  JNIEXPORT jlong JNICALL Java_jsitter_interop_JSitter_editSubtree
    (JNIEnv *env, jclass class, jlong subtree_ptr, jint start_byte, jint old_end_byte, jint new_end_byte) {
        Subtree subtree = *(Subtree *)(&subtree_ptr);
        TSInputEdit edit;
        edit.start_byte = start_byte;
        edit.old_end_byte = old_end_byte;
        edit.new_end_byte = new_end_byte;
        
        SubtreePool pool = ts_subtree_pool_new(0);
        Subtree result = ts_subtree_edit(subtree, &edit, &pool);
        ts_subtree_pool_delete(&pool);
        return *((jlong *)&result);
     }


    JNIEXPORT jlong JNICALL Java_jsitter_interop_JSitter_copyTree
    (JNIEnv *env, jclass class, jlong tree_ptr) {
        return (jlong)ts_tree_copy((TSTree *)tree_ptr);
    }

    JNIEXPORT jintArray JNICALL Java_jsitter_interop_JSitter_getChangedRanges
    (JNIEnv *env, jclass class, jlong edited_tree_ptr, jlong new_tree_ptr) {
        uint32_t length;
        TSRange *ranges = ts_tree_get_changed_ranges((TSTree *)edited_tree_ptr, (TSTree *)new_tree_ptr, &length);
        if (length == 0) {
            return NULL;
        } else {
            uint32_t *res = (uint32_t*)malloc(2 * length);
            for (uint32_t i = 0; i < length; ++i) {
                res[2*i] = ranges[i].start_byte;
                res[2*i + 1] = ranges[i].end_byte;
            }
            
            jintArray result = (*env)->NewIntArray(env, 2 * length);
            (*env)->SetIntArrayRegion(env, result, 0, 2 * length, (const jint*)res);
            return result;
        }
    }

JNIEXPORT void JNICALL Java_jsitter_interop_JSitter_parserReset
(JNIEnv * env, jclass class, jlong parser_ptr) {
  ts_parser_reset((TSParser *)parser_ptr);
}
    
    /*
     * Class:     jsitter_interop_JSitter
     * Method:    newParser
     * Signature: (J)J
     */
    JNIEXPORT jlong JNICALL Java_jsitter_interop_JSitter_newParser
    (JNIEnv * env, jclass class, jlong language_ptr, jlong cancellationFlagPtr) {
        TSParser *parser = ts_parser_new();
        ts_parser_set_cancellation_flag(parser, (size_t *)cancellationFlagPtr);
        ts_parser_set_language(parser, (TSLanguage *)language_ptr);
        return (jlong) parser;
    }
    
    JNIEXPORT jlong JNICALL JavaCritical_jsitter_interop_JSitter_newParser
    (jlong language_ptr) {
        TSParser *parser = ts_parser_new();
        ts_parser_set_language(parser, (TSLanguage *)language_ptr);
        return (jlong) parser;
    }

    JNIEXPORT jstring JNICALL Java_jsitter_interop_JSitter_toSexp
    (JNIEnv *env, jclass class, jlong language_ptr, jlong subtree_ptr) {
        const char *name = ts_subtree_string(*(Subtree *)(&subtree_ptr), (TSLanguage *)language_ptr, 0);
        return (*env)->NewStringUTF(env, name);
    }