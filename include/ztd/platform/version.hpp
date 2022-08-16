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

#ifndef ZTD_PLATFORM_VERSION_HPP
#define ZTD_PLATFORM_VERSION_HPP

#include <ztd/version.hpp>

// clang-format off

#define ZTD_PLATFORM_VERSION_MAJOR 0
#define ZTD_PLATFORM_VERSION_MINOR 0
#define ZTD_PLATFORM_VERSION_PATCH 0
#define ZTD_PLATFORM_VERSION_STRING "0.0.0"
#define ZTD_PLATFORM_VERSION \
	((ZTD_PLATFORM_VERSION_MAJOR * 100'000) + (ZTD_PLATFORM_VERSION_MINOR * 100) + (ZTD_PLATFORM_VERSION_PATCH))

#if defined(ZTD_PLATFORM_LIBICONV_STATIC)
	#if (ZTD_PLATFORM_LIBICONV_STATIC != 0)
		#define ZTD_PLATFORM_LIBICONV_STATIC_I_ ZTD_ON
	#else
		#define ZTD_PLATFORM_LIBICONV_STATIC_I_ ZTD_OFF
	#endif
#else
	#define ZTD_PLATFORM_LIBICONV_STATIC_I_ ZTD_DEFAULT_OFF
#endif

#if defined(ZTD_PLATFORM_LIBICONV_DYNAMIC)
	#if (ZTD_PLATFORM_LIBICONV_DYNAMIC != 0)
		#define ZTD_PLATFORM_LIBICONV_DYNAMIC_I_ ZTD_ON
	#else
		#define ZTD_PLATFORM_LIBICONV_DYNAMIC_I_ ZTD_OFF
	#endif
#else
	#define ZTD_PLATFORM_LIBICONV_DYNAMIC_I_ ZTD_DEFAULT_OFF
#endif

#if defined(ZTD_PLATFORM_ICONV_H)
	#if (ZTD_PLATFORM_ICONV_H != 0)
		#define ZTD_PLATFORM_ICONV_H_I_ ZTD_ON
	#else
		#define ZTD_PLATFORM_ICONV_H_I_ ZTD_OFF
	#endif
#elif ZTD_PLATFORM_HAS_INCLUDE_I_(<iconv.h>)
	#define ZTD_PLATFORM_ICONV_H_I_ ZTD_DEFAULT_ON
#else
	#define ZTD_PLATFORM_ICONV_H_I_ ZTD_DEFAULT_OFF
#endif

#if defined(ZTD_PLATFORM_LIBICONV_LOAD)
	#if (ZTD_PLATFORM_LIBICONV_LOAD != 0)
		#define ZTD_PLATFORM_LIBICONV_LOAD_I_ ZTD_ON
	#else
		#define ZTD_PLATFORM_LIBICONV_LOAD_I_ ZTD_OFF
	#endif
#else
	#define ZTD_PLATFORM_LIBICONV_LOAD_I_ ZTD_DEFAULT_OFF
#endif

#if defined(ZTD_PLATFORM_LIBICONV)
	#if (ZTD_PLATFORM_LIBICONV != 0)
		#define ZTD_PLATFORM_LIBICONV_I_ ZTD_ON
	#else
		#define ZTD_PLATFORM_LIBICONV_I_ ZTD_OFF
	#endif
#elif ZTD_IS_ON(ZTD_PLATFORM_LIBICONV_LOAD)
	#define ZTD_PLATFORM_LIBICONV_I_ ZTD_ON
#elif ZTD_IS_ON(ZTD_PLATFORM_LIBICONV_STATIC)
	#define ZTD_PLATFORM_LIBICONV_I_ ZTD_PLATFORM_LIBICONV_STATIC_I_
#elif ZTD_IS_ON(ZTD_PLATFORM_LIBICONV_DYNAMIC)
	#define ZTD_PLATFORM_LIBICONV_I_ ZTD_PLATFORM_LIBICONV_DYNAMIC_I_
#else
	#define ZTD_PLATFORM_LIBICONV_I_ ZTD_DEFAULT_OFF
#endif

#if defined(ZTD_PLATFORM_ICU)
	#if (ZTD_PLATFORM_ICU != 0)
		#define ZTD_PLATFORM_ICU_I_ ZTD_ON
	#else
		#define ZTD_PLATFORM_ICU_I_ ZTD_OFF
	#endif
#else
	#define ZTD_PLATFORM_ICU_I_ ZTD_DEFAULT_OFF
#endif

#if defined(ZTD_PLATFORM_ABI_NAMESPACE)
	#define ZTD_PLATFORM_INLINE_ABI_NAMESPACE_OPEN_I_ inline namespace ZTD_PLATFORM_ABI_NAMESPACE {
	#define ZTD_PLATFORM_INLINE_ABI_NAMESPACE_CLOSE_I_ }
#else
	#define ZTD_PLATFORM_INLINE_ABI_NAMESPACE_OPEN_I_ inline namespace __v0 {
	#define ZTD_PLATFORM_INLINE_ABI_NAMESPACE_CLOSE_I_ }
#endif

// clang-format on

#include <ztd/platform/api.hpp>

#endif // ZTD_PLATFORM_VERSION_HPP
