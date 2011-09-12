%% -----------------------------------------------------------------------------
%% https://github.com/markjeee/kannel/blob/d1d435130a24f07aaede1545e270a10398599589/gwlib/dbpool_oracle.c
%% EDBC OCI
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
%%
-module(load_env).
-compile(export_all).

main([OptFile]) ->
    Options = file:consult(OptFile),
    io:format("Writing config cache [~s]~n", [filename()]),
    file:write_file(filename(), printable(config(Options)), [write]).

config(_Options) ->
    {Alloc, Vsn, _, _} = erlang:system_info(allocator),
    [
        {otp_version, erlang:system_info(otp_release)},
        {erts_version, erlang:system_info(version)},
        {smp_available, erlang:system_info(smp_support)},
        {libc, {Alloc, string:join(Vsn, ".")}},
        {driver_vsn, erlang:system_info(driver_version)},
        {async_io, erlang:system_info(kernel_poll)}
    ].

printable(Term) ->
    erl_prettypr:format(erl_parse:abstract(Term)) ++ ".".

filename() ->
    filename:join([filename:dirname(escript:script_name()), 
                   "build", "cache", "config.cache"]).
