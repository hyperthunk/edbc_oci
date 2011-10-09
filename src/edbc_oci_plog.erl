%% -----------------------------------------------------------------------------
%%
%% EDBC OCI - port logging interface
%%
%% Copyright (c) 2011 Tim Watson (watson.timothy@gmail.com)
%%
%% Permission is hereby granted, free of charge, to any person obtaining a copy
%% of this software and associated documentation files (the "Software"), to deal
%% in the Software without restriction, including without limitation the rights
%% to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
%% copies of the Software, and to permit persons to whom the Software is
%% furnished to do so, subject to the following conditions:
%%
%% The above copyright notice and this permission notice shall be included in
%% all copies or substantial portions of the Software.
%%
%% THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
%% IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
%% FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
%% AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
%% LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
%% OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
%% THE SOFTWARE.
%% -----------------------------------------------------------------------------
-module(edbc_oci_plog).

%% These constants mirror edbc_oci_log.h
-define(ENABLE_DEBUG_LOGGING,       16#00000001).
-define(ENABLE_INFO_LOGGING,        16#00000002).
-define(ENABLE_WARN_LOGGING,        16#00000004).
-define(ENABLE_ERROR_LOGGING,       16#00000008).
-define(RESERVED_MASK_LOGGING,      16#40000000).
-define(ENABLE_SASL_LOGGING,        16#80000000).

%% we fold these options over 0 to create the correct mask
%% CommandIOList = [ <<?$(ABBREV)_ADJUST_PORT_LOGGING, (size(TagBin)):32/unsigned-integer>>, 
%%                  TagBin, 
%%                  <<OperationValue:32, Flags_FlagMask:32>> ],
%% true = erlang:port_command(ErlangPort, CommandIOList),

