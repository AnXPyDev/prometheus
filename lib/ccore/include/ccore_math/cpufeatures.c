struct {
	bool sse2;
	bool sse42;
	bool avx2;
} g_CpuFeatures = {0};

void g_getCpuFeatures(void) {
	int cpuinfo[4] = {0};
	

	bool has_xsave = false;
	bool has_osxsave = false;

	{
		__cpuid(cpuinfo, 1);
		int edx = cpuinfo[3];
		int ecx = cpuinfo[2];

		// check sse support
		g_CpuFeatures.sse2 = !!(edx & (1 << 26));
		g_CpuFeatures.sse42 = !!(ecx & (1 << 20));

		// check xgetbv support
		has_xsave = !!(ecx & (1 << 26));
		has_osxsave = !!(ecx & (1 << 27));
	}

	// check avx 2 support
	if (has_xsave && has_osxsave) {
		long long int xcr0;

		/* xgetbv(0) */ {
			int eax, edx;
			__asm__ __volatile__ (
				"xgetbv"
				: "=a"(eax), "=d"(edx)
				: "c"(0)
			);

			xcr0 = (long long int)edx << 32 | eax;
		}

		if (!!(xcr0 & ((long long int)1 << 2))) {
			__cpuid(cpuinfo, 7);
			int ebx = cpuinfo[1];

			// check avx2 support
			g_CpuFeatures.avx2 = !!(ebx & (1 << 5));
		}
	}
}

void g_printCpuFeatures(void) {
	const char *s_yes = "yes";
	const char *s_no = "no";
	fprintf(stderr, "Cpu features:\n");
	fprintf(stderr, "  SSE2: %s\n", g_CpuFeatures.sse2 ? s_yes : s_no);
	fprintf(stderr, "  SSE4.2: %s\n", g_CpuFeatures.sse42 ? s_yes : s_no);
	fprintf(stderr, "  AVX2: %s\n", g_CpuFeatures.avx2 ? s_yes : s_no);
}
