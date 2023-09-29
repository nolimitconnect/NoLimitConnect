
cmake_minimum_required (VERSION 3.22.1)

project(curl, LANGUAGES C CXX)

set(curl_SOURCES)

list(APPEND curl_SOURCES 
   lib/curl_addrinfo.c
   lib/curl_ctype.c
   lib/curl_des.c
   lib/curl_endian.c
   lib/curl_fnmatch.c
   lib/curl_gethostname.c
   lib/curl_gssapi.c
   lib/curl_memrchr.c
   lib/curl_multibyte.c
   lib/curl_ntlm_core.c
   lib/curl_ntlm_wb.c
   lib/curl_path.c
   lib/curl_range.c
   lib/curl_rtmp.c
   lib/curl_sasl.c
   lib/curl_sspi.c
   lib/curl_threads.c
   lib/ftplistparser_curl.c
   src/slist_wc_curl.c
#   src/tool_binmode.c
#   src/tool_bname.c
#   src/tool_cb_dbg.c
#   src/tool_cb_hdr.c
#   src/tool_cb_prg.c
#   src/tool_cb_rea.c
#   src/tool_cb_see.c
#   src/tool_cb_wrt.c
#   src/tool_cfgable.c
#   src/tool_convert.c
#   src/tool_dirhie.c
#   src/tool_doswin.c
#   src/tool_easysrc.c
#   src/tool_filetime.c
#   src/tool_formparse.c
#   src/tool_getparam.c
#   src/tool_getpass.c
#   src/tool_help.c
#   src/tool_helpers.c
#   src/tool_homedir.c
#   src/tool_libinfo.c
#   src/tool_main.c
#   src/tool_metalink.c
#   src/tool_mfiles.c
#   src/tool_msgs.c
#   src/tool_operate.c
#   src/tool_operhlp.c
#   src/tool_panykey.c
#   src/tool_paramhlp.c
#   src/tool_parsecfg.c
#   src/tool_setopt.c
#   src/tool_sleep.c
#   src/tool_urlglob.c
#   src/tool_util.c
#   src/tool_vms.c
#   src/tool_writeenv.c
#   src/tool_writeout.c
#   src/tool_xattr.c
   lib/url_curl.c
   lib/connect_curl.c
   lib/dict_curl.c
   lib/file_curl.c
   lib/hash_curl.c
   lib/getenv_curl.c
   lib/getinfo_curl.c
   lib/version_curl.c
   lib/ssh_curl.c
   lib/ftp_curl.c
   lib/hmac_curl.c
   lib/http_curl.c
   lib/imap_curl.c
   lib/md4_curl.c
   lib/md5_curl.c
   lib/rand_curl.c
   lib/vauth/digest_curl.c
   lib/base64_curl.c
   lib/smb_curl.c
   lib/sha256_curl.c
   lib/select_curl.c
   lib/rtsp_curl.c
   lib/strcase_curl.c
   lib/strdup_curl.c
   lib/strtok_curl.c
   lib/telnet_curl.c
   lib/setopt_curl.c
   lib/vauth/cleartext_curl.c
   lib/vauth/vauth_curl.c
   lib/vtls/gtls_curl.c
   lib/gopher_curl.c
   lib/x509asn1_curl.c
   lib/wildcard_curl.c
   lib/warnless_curl.c
   lib/transfer_curl.c
   lib/timeval_curl.c
   lib/tftp_curl.c
   lib/system_win32_curl.c
   lib/strtoofft_curl.c
   lib/strerror_curl.c
   lib/ssh-libssh_curl.c
   lib/splay_curl.c
   lib/speedcheck_curl.c
   lib/socks_sspi_curl.c
   lib/socks_gssapi_curl.c
   lib/socks_curl.c
   lib/smtp_curl.c
   lib/slist_curl.c
   lib/share_curl.c
   lib/sendf_curl.c
   lib/security_curl.c
   lib/progress_curl.c
   lib/pop3_curl.c
   lib/pipeline_curl.c
   lib/pingpong_curl.c
   lib/parsedate_curl.c
   lib/openldap_curl.c
   lib/nwos_curl.c
   lib/nwlib_curl.c
   lib/nonblock_curl.c
   lib/non-ascii_curl.c
   lib/netrc_curl.c
   lib/multi_curl.c
   lib/mprintf_curl.c
   lib/mime_curl.c
   lib/memdebug_curl.c
   lib/llist_curl.c
   lib/ldap_curl.c
   lib/krb5_curl.c
   lib/inet_pton_curl.c
   lib/inet_ntop_curl.c
   lib/if2ip_curl.c
   lib/idn_win32_curl.c
   lib/http_proxy_curl.c
   lib/http_ntlm_curl.c
   lib/http_negotiate_sspi_curl.c
   lib/http_negotiate_curl.c
   lib/http_digest_curl.c
   lib/http_chunks_curl.c
   lib/http2_curl.c
   lib/hostsyn_curl.c
   lib/hostip6_curl.c
   lib/hostip4_curl.c
   lib/hostip_curl.c
   lib/hostcheck_curl.c
   lib/hostasyn_curl.c
   lib/formdata_curl.c
   lib/fileinfo_curl.c
   lib/escape_curl.c
   lib/easy_curl.c
   lib/dotdot_curl.c
   lib/cookie_curl.c
   lib/content_encoding_curl.c
   lib/conncache_curl.c
   lib/asyn-thread_curl.c
   lib/asyn-ares_curl.c
   lib/amigaos_curl.c
   lib/vtls/vtls_curl.c
   lib/vtls/schannel_curl.c
   lib/vtls/qssl_curl.c
   lib/vtls/polarssl_threadlock_curl.c
   lib/vtls/polarssl_curl.c
   lib/vtls/openssl_curl.c
   lib/vtls/nss_curl.c
   lib/vtls/mbedtls_curl.c
   lib/vtls/gskit_curl.c
   lib/vtls/darwinssl_curl.c
   lib/vtls/cyassl_curl.c
   lib/vtls/axtls_curl.c
   lib/vauth/cram_curl.c
   lib/vauth/digest_sspi_curl.c
   lib/vauth/krb5_gssapi_curl.c
   lib/vauth/krb5_sspi_curl.c
   lib/vauth/ntlm_curl.c
   lib/vauth/ntlm_sspi_curl.c
   lib/vauth/oauth2_curl.c
   lib/vauth/spnego_gssapi_curl.c
   lib/vauth/spnego_sspi_curl.c
)

add_library(curl STATIC ${curl_SOURCES})

target_compile_definitions(curl
    PRIVATE
    BUILDING_LIBCURL
    CURL_DISABLE_LDAP
    CURL_DISABLE_TELNET
    CURL_DISABLE_DICT
    CURL_DISABLE_TFTP
    USE_SSLEA
    USE_OPENSSL
)

target_include_directories(curl
  PRIVATE
    ../openssl-1.0.2p/include
    ../openssl-1.0.2p
    include
    lib
)

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    if(CI_PIPELINE)
        # remove *.o and *.d files to avoid gitlab docker size restriction
        add_custom_command(POST_BUILD TARGET curl
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
            COMMAND find . -type f -name '*.o' -print -delete
            COMMAND find . -type f -name '*.d' -print -delete
        )
    endif()
endif()
