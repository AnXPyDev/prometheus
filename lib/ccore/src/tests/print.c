void TEST_print() {
	int x;
	PrintFmt(g_os_stdout, "hello {%i} {%p} {}", repr(int, 1231), repr(void*, &x), Vector3F_repr(Vector3F_ZERO));
}
