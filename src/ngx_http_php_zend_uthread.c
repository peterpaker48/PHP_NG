/**
 *    Copyright(c) 2016-2018 rryqszq4
 *
 *
 */

#include "ngx_php_debug.h"
#include "ngx_http_php_module.h"
#include "ngx_http_php_zend_uthread.h"

void 
ngx_http_php_zend_uthread_rewrite_inline_routine(ngx_http_request_t *r)
{
    ngx_http_php_ctx_t *ctx;
    ngx_http_php_loc_conf_t *plcf;
    ngx_str_t inline_code;

    plcf = ngx_http_get_module_loc_conf(r, ngx_http_php_module);
    ctx = ngx_http_get_module_ctx(r, ngx_http_php_module);

    ctx->phase_status = NGX_OK;

    ngx_php_request = r;

    ngx_php_set_request_status(NGX_DECLINED);

    inline_code.data = ngx_pnalloc(r->pool, sizeof("function ngx_rewrite_(){  }")-1 + ngx_strlen(plcf->rewrite_inline_code->code.string) + 32);

    inline_code.len = ngx_sprintf(inline_code.data, "function ngx_rewrite_%V(){ %*s }", 
                                        &(plcf->rewrite_inline_code->code_id), 
                                        ngx_strlen(plcf->rewrite_inline_code->code.string),
                                        plcf->rewrite_inline_code->code.string
                                    ) - inline_code.data;

    ngx_php_debug("%*s, %d", (int)inline_code.len, inline_code.data, (int)inline_code.len);

    zend_first_try {

        if (!plcf->enabled_rewrite_inline_compile){
            zend_eval_stringl_ex(
                (char *)inline_code.data, 
                inline_code.len, 
                NULL, 
                "ngx_php eval code", 
                1 
                TSRMLS_CC
            );
            plcf->enabled_rewrite_inline_compile = 1;
        }

        ngx_http_php_zend_uthread_create(r, "ngx_rewrite");

    }zend_end_try();
}

void 
ngx_http_php_zend_uthread_access_inline_routine(ngx_http_request_t *r)
{
    ngx_http_php_ctx_t *ctx;
    ngx_http_php_loc_conf_t *plcf;
    ngx_str_t inline_code;

    plcf = ngx_http_get_module_loc_conf(r, ngx_http_php_module);
    ctx = ngx_http_get_module_ctx(r, ngx_http_php_module);

    ctx->phase_status = NGX_OK;

    ngx_php_request = r;

    ngx_php_set_request_status(NGX_DECLINED);

    inline_code.data = ngx_pnalloc(r->pool, sizeof("function ngx_access_(){  }")-1 + ngx_strlen(plcf->access_inline_code->code.string) + 32);

    inline_code.len = ngx_sprintf(inline_code.data, "function ngx_access_%V(){ %*s }", 
                                        &(plcf->access_inline_code->code_id), 
                                        ngx_strlen(plcf->access_inline_code->code.string),
                                        plcf->access_inline_code->code.string
                                    ) - inline_code.data;

    ngx_php_debug("%*s, %d", (int)inline_code.len, inline_code.data, (int)inline_code.len);

    zend_first_try {

        if (!plcf->enabled_access_inline_compile){
            zend_eval_stringl_ex(
                (char *)inline_code.data, 
                inline_code.len, 
                NULL, 
                "ngx_php eval code", 
                1 
                TSRMLS_CC
            );
            plcf->enabled_access_inline_compile = 1;
        }

        ngx_http_php_zend_uthread_create(r, "ngx_access");

    }zend_end_try();
}

void 
ngx_http_php_zend_uthread_content_inline_routine(ngx_http_request_t *r)
{
    ngx_http_php_ctx_t *ctx;
    ngx_http_php_loc_conf_t *plcf;
    ngx_str_t inline_code;

    plcf = ngx_http_get_module_loc_conf(r, ngx_http_php_module);
    ctx = ngx_http_get_module_ctx(r, ngx_http_php_module);

    ctx->phase_status = NGX_OK;

    ngx_php_request = r;

    ngx_php_set_request_status(NGX_DECLINED);

    inline_code.data = ngx_pnalloc(r->pool, sizeof("function ngx_content_(){  }")-1 + ngx_strlen(plcf->content_inline_code->code.string) + 32);

    inline_code.len = ngx_sprintf(inline_code.data, "function ngx_content_%V(){ %*s }", 
                                        &(plcf->content_inline_code->code_id), 
                                        ngx_strlen(plcf->content_inline_code->code.string),
                                        plcf->content_inline_code->code.string
                                    ) - inline_code.data;

    ngx_php_debug("%*s, %d", (int)inline_code.len, inline_code.data, (int)inline_code.len);

    zend_first_try {

        if (!plcf->enabled_content_inline_compile){
            zend_eval_stringl_ex(
                (char *)inline_code.data, 
                inline_code.len, 
                NULL, 
                "ngx_php eval code", 
                1 
                TSRMLS_CC
            );
            plcf->enabled_content_inline_compile = 1;
        }
        
        ngx_http_php_zend_uthread_create(r, "ngx_content");
    
    }zend_end_try();
}

void 
ngx_http_php_zend_uthread_create(ngx_http_request_t *r, char *func_prefix)
{
    zval *func_main;
    zval *func_valid;
    zval retval;
    ngx_http_php_ctx_t *ctx;
    ngx_http_php_loc_conf_t *plcf;
    ngx_str_t func_name;

    ctx = ngx_http_get_module_ctx(r, ngx_http_php_module);

    if (ctx == NULL) {
        return ;
    }
    
    ctx->generator_closure = (zval *)emalloc(sizeof(zval));

    plcf = ngx_http_get_module_loc_conf(r, ngx_http_php_module);

    //func_name.data = ngx_pnalloc(r->pool, strlen(func_prefix)+sizeof("_18446744073709551616")-1+NGX_TIME_T_LEN);
    //func_name.len = ngx_sprintf(func_name.data, "%s_%V", func_prefix, &(plcf->content_inline_code->code_id)) - func_name.data;

    func_name.data = ngx_pnalloc(r->pool, strlen(func_prefix) + 32);

    if (strcmp(func_prefix, "ngx_rewrite") == 0) {
        func_name.len = ngx_sprintf(func_name.data, "%s_%V", func_prefix, &(plcf->rewrite_inline_code->code_id)) - func_name.data;
    }else if (strcmp(func_prefix, "ngx_access") == 0) {
        func_name.len = ngx_sprintf(func_name.data, "%s_%V", func_prefix, &(plcf->access_inline_code->code_id)) - func_name.data;
    }else if (strcmp(func_prefix, "ngx_content") == 0) {
        func_name.len = ngx_sprintf(func_name.data, "%s_%V", func_prefix, &(plcf->content_inline_code->code_id)) - func_name.data;
    }else {
        func_name.len = 0;
    }

    ngx_php_debug("%*s", (int)func_name.len, func_name.data);

    MAKE_STD_ZVAL(func_main);
    ZVAL_STRINGL(func_main, (char *)func_name.data, func_name.len, 1);
    call_user_function(EG(function_table), NULL, func_main, ctx->generator_closure, 0, NULL TSRMLS_CC);
    zval_ptr_dtor(&func_main);

    if (Z_TYPE_P(ctx->generator_closure) == IS_OBJECT){
    	MAKE_STD_ZVAL(func_valid);
        ZVAL_STRING(func_valid, "valid", 1);
        if (call_user_function(NULL, &(ctx->generator_closure), func_valid, &retval, 0, NULL TSRMLS_CC) == FAILURE)
        {
            php_error_docref(NULL TSRMLS_CC, E_WARNING, "Failed calling valid");
            return ;
        }
        zval_ptr_dtor(&func_valid);

        ngx_php_debug("r:%p, closure:%p, retval:%d", r, ctx->generator_closure, Z_TYPE(retval));

        if (Z_TYPE(retval) == IS_BOOL && Z_BVAL(retval)){
            ctx->phase_status = NGX_AGAIN;
        }else {
            ctx->phase_status = NGX_OK;
        }

    }else {
        ngx_php_debug("r:%p, closure:%p, retval:%d", r, ctx->generator_closure, Z_TYPE(retval));
        efree(ctx->generator_closure);
    }
}

void 
ngx_http_php_zend_uthread_resume(ngx_http_request_t *r)
{
    ngx_php_request = r;

    ngx_http_php_ctx_t *ctx = ngx_http_get_module_ctx(r, ngx_http_php_module);

    ngx_php_debug("ctx: %p", ctx);

    if (ctx == NULL) {
        
    }

    zend_try {
        zval *closure;
        zval *func_next;
        zval *func_valid;
        zval retval;

        closure = ctx->generator_closure;

        MAKE_STD_ZVAL(func_valid);
        ZVAL_STRING(func_next, "next", 1);
        call_user_function(NULL, &(closure), func_next, &retval, 0, NULL TSRMLS_CC);
        zval_ptr_dtor(&func_next);

        MAKE_STD_ZVAL(func_valid);
        ZVAL_STRING(func_valid, "valid", 1);
        call_user_function(NULL, &(closure), func_valid, &retval, 0, NULL TSRMLS_CC);
        zval_ptr_dtor(&func_valid);

        ngx_php_debug("r:%p, closure:%p, retval:%d", r, closure, Z_TYPE(retval));

        if (Z_TYPE(retval) == IS_BOOL && Z_BVAL(retval)) {
            ctx->phase_status = NGX_AGAIN;
        }else {
            ctx->phase_status = NGX_OK;
            ngx_http_core_run_phases(r);
            efree(ctx->generator_closure);
            ctx->generator_closure = NULL;
        }

    }zend_catch {

    }zend_end_try();
}

void 
ngx_http_php_zend_uthread_exit(ngx_http_request_t *r)
{
    ngx_http_php_ctx_t *ctx;

    ngx_php_request = r;

    ctx = ngx_http_get_module_ctx(r, ngx_http_php_module);

    if (ctx == NULL) {

    }

    if (ctx->generator_closure) {
        //ngx_http_php_zend_uthread_resume(r);
        ctx->phase_status = NGX_OK;
        ngx_http_core_run_phases(r);
        efree(ctx->generator_closure);
        ctx->generator_closure = NULL;
    }

}
