FileOutStream g_fos_stderr;
OutStream g_os_stderr;
FileOutStream g_fos_stdout;
OutStream g_os_stdout;
FileInStream g_fis_stdin;
InStream g_is_stdin;

void g_initStdStreams(void) {
    g_fos_stderr = FileOutStream_new(stderr);
    g_fos_stdout = FileOutStream_new(stdout);
    g_fis_stdin = FileInStream_new(stdin);

    g_os_stderr = FileOutStream_upcast(&g_fos_stderr);
    g_os_stdout = FileOutStream_upcast(&g_fos_stdout);
    g_is_stdin = FileInStream_upcast(&g_fis_stdin);
}
