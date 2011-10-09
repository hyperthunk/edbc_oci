%% -----------------------------------------------------------------------------
%%
%% EDBC OCI - port controller
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
-module(edbc_oci_pctrl).
-behaviour(gen_server).

-define(SERVER, ?MODULE).

%% OTP Exports
-export([init/1, handle_call/3, handle_cast/2,
                 handle_info/2, terminate/2, code_change/3]).

%% Public API Exports
-export([start/0, start_link/0, start/1,
                 start_link/1, stop/0]).

%% Lib defines
-define(DRIVER_NAME, "liboci_drv").

%% Magics...
-define(PORT_INIT, 9).

%%
%% Public API
%%

start() ->
    start(application:get_all_env(edbc_oci)).

%% @doc starts erlxsl_port_server with default options.
start(Config) ->
    gen_server:start({local, ?SERVER}, ?SERVER, Config, []).

start_link() ->
    start_link(application:get_all_env(edbc_oci)).

%% @doc starts erlxsl_port_server with default options.
start_link(Config) ->
    gen_server:start_link({local,?SERVER}, ?SERVER, Config, []).

stop() ->
    gen_server:cast(?SERVER, stop).

%% gen_server api

init(Config) ->
    process_flag(trap_exit, true),
    erl_ddll:start(),
    BinPath = filename:join(code:lib_dir(edbc_oci, priv), "bin"),
    case erl_ddll:load_driver(BinPath, ?DRIVER_NAME) of
        {error, Error} ->
            {stop, {Error, erl_ddll:format_error(Error)}};
        ok ->
            Port = open_port({spawn, ?DRIVER_NAME}, [binary]),
            init_port(Port, Config)
    end.

handle_call(_Request, _From, State) ->
    {noreply, ok, State}.

handle_cast(_Msg, State) ->
    {noreply, State}.

handle_info(_Info, State) ->
    {noreply, State}.

terminate(_Reason, _State) ->
    ok.

code_change(_OldVsn, State, _Extra) ->
    {ok, State}.

%% ------------------------------------------------------------------
%% Internal Function Definitions
%% ------------------------------------------------------------------

init_port(_Port, _Config) ->
    %% port_command(Port, edbc_oci_pdpm:pack(Config)),
    %% receive
    %%    Data -> gen_server:reply(Client, Data)
    %%end.
    ok.
