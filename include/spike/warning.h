/*
----------------------------------------------------------------------

BSD 3-Clause License

Copyright (c) 2026, Cayden Andrews

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

----------------------------------------------------------------------
*/

#ifndef __SPIKE_WARNING_H
#define __SPIKE_WARNING_H

#define STRINGIFY(X) #X

#if defined(_MSC_VER)

#define DISABLE_WARNING_PUSH __pragma(warning(push))
#define DISABLE_WARNING(war) __pragma(warning(disable : war))
#define DISABLE_WARNING_POP __pragma(warning(pop))

#define DISABLE_WARNING_UNREFERENCED_FORMAL_PARAMETER	DISABLE_WARNING(4100)
#define DISABLE_WARNING_UNREFERENCED_FUNCTION			DISABLE_WARNING(4505)
#define DISABLE_WARNING_UNUSED_VARIABLE 				DISABLE_WARNING(26444)
#define DISABLE_WARNING_UNINITIALIZED_VARIABLE			DISABLE_WARNING(26495)
#define DISABLE_WARNING_ENUM_INCOMPLETE					DISABLE_WARNING(4715)

#define DISABLE_WARNING_YAML_H 							DISABLE_WARNING(4251) DISABLE_WARNING(4275)

#elif defined(__GNUC__) || defined(__clang__)

#define DISABLE_WARNING_PUSH _Pragma("GCC diagnostic push")
#define DISABLE_WARNING(war) _Pragma(STRINGIFY(GCC diagnostic ignored #war))
#define DISABLE_WARNING_POP _Pragma("GCC diagnostic pop")

#define DISABLE_WARNING_UNREFERENCED_FORMAL_PARAMETER    DISABLE_WARNING(-Wunused-parameter)
#define DISABLE_WARNING_UNREFERENCED_FUNCTION            DISABLE_WARNING(-Wunused-function)
#define DISABLE_WARNING_UNUSED_VARIABLE
#define DISABLE_WARNING_UNINITIALIZED_VARIABLE

#define DISABLE_WARNING_YAML_H

#else

#define DISABLE_WARNING_PUSH
#define DISABLE_WARNING(war)
#define DISABLE_WARNING_POP

#define DISABLE_WARNING_UNREFERENCED_FORMAL_PARAMETER
#define DISABLE_WARNING_UNREFERENCED_FUNCTION
#define DISABLE_WARNING_UNUSED_VARIABLE
#define DISABLE_WARNING_UNINITIALIZED_VARIABLE

#define DISABLE_WARNING_YAML_H

#endif

#endif