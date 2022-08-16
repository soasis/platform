// =============================================================================
//
// ztd.platform
// Copyright © 2022 JeanHeyd "ThePhD" Meneide and Shepherd's Oasis, LLC
// Contact: opensource@soasis.org
//
// Commercial License Usage
// Licensees holding valid commercial ztd.platform licenses may use this file in
// accordance with the commercial license agreement provided with the
// Software or, alternatively, in accordance with the terms contained in
// a written agreement between you and Shepherd's Oasis, LLC.
// For licensing terms and conditions see your agreement. For
// further information contact opensource@soasis.org.
//
// Apache License Version 2 Usage
// Alternatively, this file may be used under the terms of Apache License
// Version 2.0 (the "License") for non-commercial use; you may not use this
// file except in compliance with the License. You may obtain a copy of the
// License at
//
//		http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// ============================================================================ //

#include <ztd/platform/iconv.hpp>

#include <ztd/idk/detail/windows.hpp>
#include <ztd/idk/detail/posix.hpp>

#if ZTD_IS_ON(ZTD_PLATFORM_LIBICONV)

#if ZTD_IS_ON(ZTD_PLATFORM_ICONV_H)
#include <iconv.h>
#elif ZTD_IS_ON(ZTD_PLATFORM_LIBICONV_LOAD)
#if ZTD_IS_ON(ZTD_PLATFORM_POSIX)
#elif ZTD_IS_ON(ZTD_PLATFORM_WINDOWS)
// taken care of above
#else
#error "[ztd.plat] Cannot find a runtime loading mechanism to use for this platform!"
#endif
#endif

#include <array>

#include <ztd/prologue.hpp>

namespace ztd { namespace plat {

	ZTD_PLATFORM_INLINE_ABI_NAMESPACE_OPEN_I_

	namespace icnv {

		namespace __icv_detail {

			ZTD_PLATFORM_API_LINKAGE_I_ __startup::__startup() noexcept
			: convert(nullptr)
			, open(nullptr)
			, close(nullptr)
#if ZTD_IS_ON(ZTD_PLATFORM_LIBICONV_LOAD)
			, _M_handle(nullptr)
#endif
			{
#if ZTD_IS_ON(ZTD_PLATFORM_LIBICONV_LOAD)
				constexpr ::std::array<const char*, 2> __convert_names = { { "iconv", "_iconv" } };
				constexpr ::std::array<const char*, 2> __open_names    = { { "iconv_open", "_iconv_open" } };
				constexpr ::std::array<const char*, 2> __close_names   = { { "iconv_close", "_iconv_close" } };
				constexpr ::std::array<const char*, 11> __lib_names { { "libiconv.so", "iconv.so", "libiconv",
					"iconv", "iconv.dll", "iconv-2.dll", "libiconv.dll", "libiconv2.dll", "libiconv-2.dll",
					"libc.so", "libc" } };
				for (::std::size_t __index = 0; __index < std::size(__lib_names); ++__index) {
					const char* __lib_name = __lib_names[__index];
#if ZTD_IS_ON(ZTD_PLATFORM_POSIX) && ZTD_IS_ON(ZTD_DLFCN_H)
					this->_M_handle = dlopen(__lib_name, RTLD_LAZY);
#elif ZTD_IS_ON(ZTD_PLATFORM_WINDOWS)
					HMODULE __current_module = NULL;
					HRESULT __module_result  = GetModuleHandleEx(0, __lib_name, &__current_module);
					if (!SUCCEEDED(__module_result)) {
						continue;
					}
					static_assert(sizeof(void*) >= sizeof(HMODULE));
					this->_M_handle = reinterpret_cast<void*>(__current_module);
#else
#error "Not yet implemented, message Shepherd's Oasis about missing functionality for your platform!"
#endif
					if (this->_M_handle == nullptr) {
						continue;
					}
					for (::std::size_t __convert_index = 0; __convert_index < __convert_names.size();
						++__convert_index) {
						const char* __convert_name = __convert_names[__convert_index];
#if ZTD_IS_ON(ZTD_PLATFORM_POSIX) && ZTD_IS_ON(ZTD_DLFCN_H)
						void* __convert_f = dlsym(this->_M_handle, __convert_name);
#elif ZTD_IS_ON(ZTD_PLATFORM_WINDOWS)
						void* __convert_f = reinterpret_cast<void*>(
							GetProcAddress(reinterpret_cast<HMODULE>(this->_M_handle), __convert_name));
#endif
						if (__convert_f == nullptr) {
							continue;
						}
						this->convert = reinterpret_cast<convert_function*>(__convert_f);
						break;
					}
					for (::std::size_t __open_index = 0; __open_index < __open_names.size(); ++__open_index) {
						const char* __open_name = __open_names[__open_index];
#if ZTD_IS_ON(ZTD_PLATFORM_POSIX) && ZTD_IS_ON(ZTD_DLFCN_H)
						void* __open_f = dlsym(this->_M_handle, __open_name);
#elif ZTD_IS_ON(ZTD_PLATFORM_WINDOWS)
						void* __open_f = reinterpret_cast<void*>(
							GetProcAddress(reinterpret_cast<HMODULE>(this->_M_handle), __open_name));
#endif
						if (__open_f == nullptr) {
							continue;
						}
						this->open = reinterpret_cast<open_function*>(__open_f);
						break;
					}
					for (::std::size_t __close_index = 0; __close_index < __close_names.size(); ++__close_index) {
						const char* __close_name = __close_names[__close_index];
#if ZTD_IS_ON(ZTD_PLATFORM_POSIX) && ZTD_IS_ON(ZTD_DLFCN_H)
						void* __close_f = dlsym(this->_M_handle, __close_name);
#elif ZTD_IS_ON(ZTD_PLATFORM_WINDOWS)
						void* __close_f = reinterpret_cast<void*>(
							GetProcAddress(reinterpret_cast<HMODULE>(this->_M_handle), __close_name));
#endif
						if (__close_f == nullptr) {
							continue;
						}
						this->close = reinterpret_cast<close_function*>(__close_f);
						break;
					}
					if (this->open == nullptr || this->close == nullptr || this->convert == nullptr) {
						this->open    = nullptr;
						this->close   = nullptr;
						this->convert = nullptr;
					}
					else {
						// we've got everything: leave!
						break;
					}
					if (this->_M_handle != nullptr) {
#if ZTD_IS_ON(ZTD_PLATFORM_POSIX) && ZTD_IS_ON(ZTD_DLFCN_H)
						int __close_handle_result = dlclose(this->_M_handle);
						if (__close_handle_result == 0) {
							// freak out??
						}
#elif ZTD_IS_ON(ZTD_PLATFORM_WINDOWS)
						BOOL __close_handle_result = CloseHandle(reinterpret_cast<HMODULE>(this->_M_handle));
						if (__close_handle_result == 0) {
							// freak out??
						}
#endif
						this->_M_handle = nullptr;
					}
				}
#else
				this->convert = reinterpret_cast<convert_function*>(&iconv);
				this->open    = reinterpret_cast<open_function*>(&iconv_open);
				this->close   = reinterpret_cast<close_function*>(&iconv_close);
#endif
			}

			ZTD_PLATFORM_API_LINKAGE_I_ bool __startup::is_valid() const noexcept {
				return this->convert != nullptr && this->open != nullptr && this->close != nullptr
#if ZTD_IS_ON(ZTD_PLATFORM_LIBICONV_LOAD)
					&& this->_M_handle != nullptr
#endif
					;
			}

			ZTD_PLATFORM_API_LINKAGE_I_ __startup::~__startup() {
#if ZTD_IS_ON(ZTD_PLATFORM_LIBICONV_LOAD)
#if ZTD_IS_ON(ZTD_PLATFORM_POSIX) && ZTD_IS_ON(ZTD_DLFCN_H)
				int __close_handle_result = dlclose(this->_M_handle);
				if (__close_handle_result == 0) {
					// freak out??
				}
#elif ZTD_IS_ON(ZTD_PLATFORM_WINDOWS)
				BOOL __close_handle_result = CloseHandle(reinterpret_cast<HMODULE>(this->_M_handle));
				if (__close_handle_result == 0) {
					// freak out??
				}
#endif
#endif
			}
		} // namespace __icv_detail

		ZTD_PLATFORM_API_LINKAGE_I_ bool descriptor_is_valid(descriptor __desc) noexcept {
			if constexpr (::std::is_pointer_v<descriptor>) {
				if (__desc == nullptr) {
					return false;
				}
			}
			return __desc != failure_descriptor;
		}

		ZTD_PLATFORM_API_LINKAGE_I_ const __icv_detail::__startup& functions() noexcept {
			static __icv_detail::__startup __funcs {};
			return __funcs;
		}
	} // namespace icnv

	ZTD_PLATFORM_INLINE_ABI_NAMESPACE_CLOSE_I_

}} // namespace ztd::plat

#include <ztd/epilogue.hpp>

#endif
