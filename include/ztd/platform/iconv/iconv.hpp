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

#pragma once

#ifndef ZTD_PLATFORM_ICONV_ICONV_HPP
#define ZTD_PLATFORM_ICONV_ICONV_HPP

#include <ztd/platform/version.hpp>

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

#include <cstddef>

#include <ztd/prologue.hpp>

namespace ztd { namespace plat {

	ZTD_PLATFORM_INLINE_ABI_NAMESPACE_OPEN_I_

	namespace icnv {

#if ZTD_IS_ON(ZTD_PLATFORM_ICONV_H)
		using descriptor = iconv_t;
#else
		using descriptor = void*;
#endif

		using convert_function = size_t(descriptor, const char**, ::std::size_t*, char**, ::std::size_t*) noexcept;
		using open_function    = descriptor(const char*, const char*) noexcept;
		using close_function   = int(descriptor) noexcept;

		namespace __icv_detail {

			class __startup {
			public:
				convert_function* convert;
				open_function* open;
				close_function* close;

				ZTD_PLATFORM_API_LINKAGE_I_ __startup() noexcept;
				ZTD_PLATFORM_API_LINKAGE_I_ bool is_valid() const noexcept;
				ZTD_PLATFORM_API_LINKAGE_I_ ~__startup();

			private:
#if ZTD_IS_ON(ZTD_PLATFORM_LIBICONV_LOAD)
				void* _M_handle;
#endif
			};
		} // namespace __icv_detail

		inline const descriptor failure_descriptor = reinterpret_cast<descriptor>(static_cast<::std::size_t>(-1));
		inline constexpr ::std::size_t conversion_failure = static_cast<::std::size_t>(-1);
		inline constexpr ::std::size_t conversion_success = static_cast<::std::size_t>(0);
		inline constexpr int close_failure                = static_cast<int>(-1);
		inline constexpr int close_success                = static_cast<int>(0);

		ZTD_CXX_LANGUAGE_LINKAGE_I_ ZTD_PLATFORM_API_LINKAGE_I_ bool descriptor_is_valid(descriptor __desc) noexcept;

		ZTD_CXX_LANGUAGE_LINKAGE_I_ ZTD_PLATFORM_API_LINKAGE_I_ const __icv_detail::__startup& functions() noexcept;

	} // namespace icnv

	ZTD_PLATFORM_INLINE_ABI_NAMESPACE_CLOSE_I_

}} // namespace ztd::plat

#include <ztd/epilogue.hpp>

#endif

#endif
