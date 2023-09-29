
cmake_minimum_required (VERSION 3.22.1)

project(ssh, LANGUAGES C CXX)

set(ssh_SOURCES)

list(APPEND ssh_SOURCES 
    poll_ssh.c
    options_ssh.c
    error_ssh.c
    external/bcrypt_pbkdf_ssh.c
    external/blowfish_ssh.c
    external/curve25519_ref_ssh.c
    external/ed25519_ssh.c
    external/fe25519_ssh.c
    external/ge25519_ssh.c
    external/sc25519_ssh.c
    threads/pthread_ssh.c
    agent_ssh.c
    auth_ssh.c
    auth1_ssh.c
    base64_ssh.c
    bignum_ssh.c
    bind_ssh.c
    buffer_ssh.c
    callbacks_ssh.c
    channels_ssh.c
    channels1_ssh.c
    client_ssh.c
    config_ssh.c
    connect_ssh.c
    crc32_ssh.c
    curve25519_ssh.c
    dh_ssh.c
    ecdh_ssh.c
    gcrypt_missing_ssh.c
    getpass_ssh.c
    gssapi_ssh.c
    gzip_ssh.c
    init_ssh.c
    kex_ssh.c
    kex1_ssh.c
    known_hosts_ssh.c
    legacy_ssh.c
    libcrypto_ssh.c
    libgcrypt_ssh.c
    libssh_string_ssh.c
    log_ssh.c
    match_ssh.c
    messages_ssh.c
    misc_ssh.c
    packet_ssh.c
    packet1_ssh.c
    packet_cb_ssh.c
    packet_crypt_ssh.c
    pcap_ssh.c
    pki_ssh.c
    pki_container_openssh_ssh.c
    pki_crypto_ssh.c
    pki_ed25519_ssh.c
    pki_gcrypt_ssh.c
    scp_ssh.c
    server_ssh.c
    session_ssh.c
    sftp_ssh.c
    sftpserver_ssh.c
    socket_ssh.c
    threads_ssh.c
    wrapper_ssh.c
)

add_library(ssh STATIC ${ssh_SOURCES})

target_compile_definitions(ssh
    PRIVATE
    LIBSSH_STATIC
)

#TODO these are from openssl.. needs to be in a common cmake file
target_compile_definitions(ssh
    PRIVATE
    _LIB
    MONOLITH
    OPENSSL_THREADS
    L_ENDIAN
    OPENSSL_BN_ASM_PART_WORDS
    OPENSSL_NO_RC5
    OPENSSL_NO_MD2
    OPENSSL_NO_KB5
    OPENSSL_NO_JPAKE
    OPENSSL_NO_DYNAMIC_ENGINE
    NOMINMAX
    NO_WINDOWS_BRAINDEATH
)

if(DEFINED TARGET_OS_WINDOWS)
    target_compile_definitions(ssh
        PRIVATE
        OPENSSL_IA32_SSE2
    )
endif()

if(DEFINED TARGET_OS_LINUX)
    target_compile_definitions(ssh
        PRIVATE
        OPENSSL_IA32_SSE2
    )
endif()

target_include_directories(ssh
  PRIVATE
    ../openssl-1.0.2p/include
    ../openssl-1.0.2p/crypto
    ../openssl-1.0.2p/crypto/modes
    ../openssl-1.0.2p/crypto/asn1
    ../openssl-1.0.2p/crypto/evp
)

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    if(CI_PIPELINE)
        # remove *.o and *.d files to avoid gitlab docker size restriction
        add_custom_command(POST_BUILD TARGET ssh
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
            COMMAND find . -type f -name '*.o' -print -delete
            COMMAND find . -type f -name '*.d' -print -delete
        )
    endif()
endif()

