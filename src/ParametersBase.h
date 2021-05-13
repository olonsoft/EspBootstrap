#ifndef _PARAMETERSBASE_H_
#define _PARAMETERSBASE_H_

/*
  Copyright (c) 2015-2020, Anatoli Arkhipenko.
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification,
  are permitted provided that the following conditions are met:

  1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

  2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

  3. Neither the name of the copyright holder nor the names of its contributors
   may be used to endorse or promote products derived from this software without
   specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
  OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
*/

#include <Arduino.h>

enum class ParametersResult {
  ParamsOk,
  ParamsError,
  ParamsLen,
  ParamsCRC,
  ParamsTok,
  ParamsFDE,
  ParamsMem,
  ParamsAct  
};

// Error codes:
/* #define PARAMS_OK   0
#define PARAMS_ERR  (-1)
#define PARAMS_LEN  (-2)
#define PARAMS_CRC  (-3)
#define PARAMS_TOK  (-4)
#define PARAMS_FDE  (-5)
#define PARAMS_MEM  (-98)
#define PARAMS_ACT  (-99) */


class ParametersBase {
  public:
    ParametersBase(const String& aToken);
    virtual ~ParametersBase();

    virtual ParametersResult  begin() = 0;
    virtual ParametersResult  load() = 0;
    virtual ParametersResult  save() = 0;

    virtual inline int8_t isActive() {
      return iActive;
    };

  protected:
    int8_t          iActive;
    const String&   iToken;
};

ParametersBase::ParametersBase(const String& aToken) : iToken(aToken) {
  iActive = false;
}

ParametersBase::~ParametersBase () {}

#endif // _PARAMETERSBASE_H_