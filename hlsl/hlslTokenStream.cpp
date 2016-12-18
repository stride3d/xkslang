//
//Copyright (C) 2016 Google, Inc.
//
//All rights reserved.
//
//Redistribution and use in source and binary forms, with or without
//modification, are permitted provided that the following conditions
//are met:
//
//    Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
//    Redistributions in binary form must reproduce the above
//    copyright notice, this list of conditions and the following
//    disclaimer in the documentation and/or other materials provided
//    with the distribution.
//
//    Neither the name of Google, Inc., nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
//THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
//FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
//COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
//INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
//BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
//CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
//LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
//ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//POSSIBILITY OF SUCH DAMAGE.
//

#include "hlslTokenStream.h"

namespace glslang {

	void HlslTokenStream::pushTokenBuffer(const HlslToken& tok)
	{
		assert(tokenBufferPos == tokenBuffer.size());  //we cannot push if we're tracking back to a previous position

		tokenBuffer.push_back(tok);
		tokenBufferPos++;
	}

	HlslToken HlslTokenStream::popTokenBuffer()
	{
		// Back up
		assert(tokenBufferPos > 0);

		tokenBufferPos--;
		return tokenBuffer[tokenBufferPos];
	}

	bool HlslTokenStream::recedeToToken(HlslToken tok)
	{
		if (token.IsEqualsToToken(tok)) return false;

		//find the token in our list of accepted token
		int tokenIndex = -1;
		for (int i = 0; i < tokenBufferPos; ++i)
		{
			if (tokenBuffer[i].IsEqualsToToken(tok))
			{
				tokenIndex = i;
				break;
			}
		}
		if (tokenIndex == -1) return false;

		if (tokenBufferPos == tokenBuffer.size())
		{
			//save current token at the end of buffer, so that we can push it back
			pushTokenBuffer(token);
		}
		tokenBufferPos = tokenIndex;
		token = tokenBuffer[tokenBufferPos];
		return true;
	}

	// Load 'token' with the next token in the stream of tokens.
	void HlslTokenStream::advanceToken()
	{
		//Save the current token if need, or increase the buffer position
		if (tokenBufferPos == tokenBuffer.size())
		{
			pushTokenBuffer(token);	
		}
		else
		{
			tokenBufferPos++;
		}

		//Get the next token
		if (tokenBufferPos == tokenBuffer.size())
		{
			scanner.tokenize(token);
		}
		else
		{
			token = tokenBuffer[tokenBufferPos];
		}
	}

	bool HlslTokenStream::recedeToken()
	{
		if (tokenBufferPos == 0) return false;

		if (tokenBufferPos == tokenBuffer.size())
		{
			//save current token at the end of buffer, so that we can push it back
			pushTokenBuffer(token);
			tokenBufferPos--;
		}

		token = popTokenBuffer();

		return true;
	}

	// Return the current token class.
	EHlslTokenClass HlslTokenStream::peek() const
	{
		return token.tokenClass;
	}

	// Return true, without advancing to the next token, if the current token is
	// the expected (passed in) token class.
	bool HlslTokenStream::peekTokenClass(EHlslTokenClass tokenClass) const
	{
		return peek() == tokenClass;
	}

	// Return true and advance to the next token if the current token is the
	// expected (passed in) token class.
	bool HlslTokenStream::acceptTokenClass(EHlslTokenClass tokenClass)
	{
		if (peekTokenClass(tokenClass)) {
			advanceToken();
			return true;
		}

		return false;
	}

} // end namespace glslang
