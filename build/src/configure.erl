%% -----------------------------------------------------------------------------
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
-module(configure).

-include_lib("libconf/include/libconf.hrl").
-compile(export_all).

main(Args) ->
    libconf:configure(Args, options(), rules()).

options() ->
    [
        {"--(?<option>.*)=(?<value>.*)", fun erlang:list_to_tuple/1,
            [option, value],
            [{"prefix", "Base install directory", env:code_dir()},
             {"erlang", "Path to erl, erlc and escript", code:root_dir()}]},
        {"--enable-(?<thing>.*)", [enabled], [thing],
            [{"arch64bit", "Enable 64bit mode (Arch Dependant)", undefined},
             {"darwin_64bit",
                "Defaults to 32bit OCI builds", disabled},
             {"drcp", "Enable DRCP (OCI >= 11g only)", undefined},
             {"debug", "Enable debug builds", disable}]},
        {"--disable-(?<thing>.*)", [disabled], [thing],
            [{"arch64bit", "Force 32bit build on 64bit platforms", undefined},
             {"drcp", "Disable Database Resident Connection Pooling "
                      "(default OCI < 11g)", undefined}]},
        {"--with-(?<thing>.*)=(?<where>.*)", fun erlang:list_to_tuple/1,
                [thing, where],
            [{"oci", "Location of the OCI install", "/opt/usr/oracle"},
             {"oracle_home", "Override ORACLE_HOME environment variable",
                "/usr/local/oracle"},
             {"cc", "Full path to C Compiler (Platform Dependant)",
                undefined},
             {"rebar", "Location of rebar binary", "$HOME/bin/rebar"}]}
    ].

rules() ->
    [{checks, [
        #check{ name=oci, type=library, mandatory=true,
                capture="printf(\"%i.%i\", (int)OCI_MAJOR_VERSION,"
                        " (int)OCI_MINOR_VERSION);",
                data=#require{
                    include="oci.h",
                    find="libclntsh",
                    path="${oci}",
                    incl_path=["${oci}/sdk/include"],
                    code_path=["${oci}"] }},
        #check{ name=stdarg, type=include, mandatory=false,
                output="_EDBC_OCI_HAVE_STDARG",
                data=#require{ include="stdarg.h" }},
        #check{ name=stdint, type=include, mandatory=true,
                output="_EDBC_OCI_HAVE_STDINT",
                data=#require{ include="stdint.h" }}
    ]},
    {templates, [
        #template{ name=config, 
                   pre_render=config_h,
                   output="c_src/config.h",
                   module=cconfig_h_template, 
                   checks=[stdarg, stdint] },
        #template{ name=main_config,
                   pre_render=?MODULE,
                   output="rebar.config",
                   checks=[oci], data=[{oci, data}],
                   defaults=[
                        {source_files, [<<"\"c_src/*.c\"">>]},
                        {object_files, [<<"\"c_src/*.o\"">>]},
                        {arch_32, "%{cc:calculate_arch_flags('x86')}"},
                        {arch_64,
                            "%{cc:calculate_arch_flags(environment.wordsize)}"}
                   ]},
       #template{ name=test_config,
                  pre_render=?MODULE,
                  output="test.config",
                  checks=[oci], data=[{oci, data}],
                  defaults=[
                        {additional_includes, [<<"spec">>, 
                                               <<"cspec/src">>]},
                        {source_files, [<<"\"c_src/*.c\"">>]},
                        {object_files, [<<"\"c_src/*.o\"">>]},
                        {arch_32, "%{cc:calculate_arch_flags('x86')}"},
                        {arch_64,
                            "%{cc:calculate_arch_flags(environment.wordsize)}"}
                  ]},
        #template{ name=makefile,
                   module=makefile_template,
                   output="Makefile",
                   overwrite=true,
                   defaults=[{rebar, "${options.rebar}"}] }
    ]}].

pre_render(T=#template{ output=OutFile }, _, _, _) ->
    %% TODO: add filesys opers to libconf
    file:delete(OutFile), T.
