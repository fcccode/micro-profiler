#include <test-helpers/helpers.h>

#define __STDC_FORMAT_MACROS

#include <common/path.h>
#include <dlfcn.h>
#include <link.h>
#include <inttypes.h>
#include <stdio.h>
#include <unistd.h>

using namespace std;

namespace micro_profiler
{
	namespace tests
	{
		namespace
		{
			void *find_any_mapped_for(const string &name)
			{
				shared_ptr<FILE> f(fopen("/proc/self/maps", "r"), &fclose);
				char line[1000] = { 0 };

				while (fgets(line, sizeof(line) - 1, f.get()))
				{
					void *from, *to, *offset;
					char rights[10], path[1000] = { 0 };
					unsigned dummy;

					if (sscanf(line, "%" SCNxPTR "-%" SCNxPTR " %s %" SCNxPTR " %x:%x %d %s\n",
						&from, &to, rights, &offset, &dummy, &dummy, &dummy, path) > 0 && path[0])
					{
						if (string(path).find(name) != string::npos)
							return from;
					}
				}
				return 0;
			}

			void release_module(void *h)
			{
				if (h)
					::dlclose(h);
			}

			string make_dlpath(const string &from)
			{
				if (from.find('/') == string::npos)
					return "./" & from;
				return from;
			}
		}

		string get_current_process_executable()
		{
			int n;
			char path[1000] = {};

			if (n = ::readlink("/proc/self/exe", path, sizeof(path) - 1), n == -1)
				return string();
			path[n] = 0;
			return path;
		}


		image::image(const char *path_)
		{
			string path(path_);

			if (path.find(".so") == string::npos)
				path = path + ".so";
			reset(::dlopen(make_dlpath(path).c_str(), RTLD_NOW), &release_module);
			if (!get())
			{
				reset(::dlopen(make_dlpath("lib" + path).c_str(), RTLD_NOW), &release_module);
				if (!get())
					throw runtime_error("Cannot load module specified!");
			}

			void *addr = find_any_mapped_for(*path);

			Dl_info di = { };

			::dladdr(addr, &di);
			_base = static_cast<byte *>(di.dli_fbase);
			_fullpath = di.dli_fname;
		}

		byte *image::load_address_ptr() const
		{	return _base;	}

		long_address_t image::load_address() const
		{	return reinterpret_cast<size_t>(load_address_ptr());	}

		const char *image::absolute_path() const
		{	return _fullpath.c_str();	}

		void *image::get_symbol_address(const char *name) const
		{
			if (void *symbol = ::dlsym(get(), name))
				return symbol;
			throw runtime_error("Symbol specified was not found!");
		}

		unsigned image::get_symbol_rva(const char *name) const
		{
			if (void *symbol = ::dlsym(get(), name))
				return static_cast<byte *>(symbol) - load_address_ptr();
			throw runtime_error("Symbol specified was not found!");
		}
	}
}
