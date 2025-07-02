#include <nodepp/nodepp.h>
#include <nodepp/cluster.h>
#include <nodepp/popen.h>
#include <nodepp/timer.h>
#include <nodepp/http.h>
#include <nodepp/fs.h>
#include <nodepp/os.h>

/*────────────────────────────────────────────────────────────────────────────*/

using namespace nodepp;

/*────────────────────────────────────────────────────────────────────────────*/

void show_main_url() { timer::timeout([=](){

    auto dir=string_t( "/tmp/tor_node_main" );
    auto fid=regex::format( "${0}/hidden/hostname",dir );
    auto url=regex::format( "http://${0}", fs::read_file(fid).slice(0,-1) );
    
    console::log( "<>", url );

},1000); }

/*────────────────────────────────────────────────────────────────────────────*/

void spawn( uint cpu ) { try {

    auto env=regex::format( "?CPU=${0}", cpu ); 
    auto pid=cluster::add ( ptr_t<string_t>({ env }) ); 

    pid.onDrain.once([=](){
        if( process::is_exit() ){ return; }
        console::error( "< spawned >" ); spawn( cpu );
    });

} catch(...) {} }

void create_load_balance() { try {

    for( auto cpu=os::cpus(); cpu-->0; )
       { spawn( cpu ); }

} catch(...) { 
    console::error("something went wrong");
    process::exit (1); 
}}

/*────────────────────────────────────────────────────────────────────────────*/

void create_http_redirector(){ process::delay(1000); try {

    auto cpu=process::env::get("CPU");

    auto dir=regex::format( "/tmp/tor_node_${0}"  ,cpu );
    auto fid=regex::format( "${0}/hidden/hostname",dir );
    auto url=regex::format( "http://${0}", fs::read_file(fid).slice(0,-1) );
 
    auto server = http::server([=]( http_t cli ){
        cli.write_header( 302, header_t({ 
            { "Location", url } 
        })); cli.close();
    });

    server.onClose.once([=](){
        if( process::is_exit() ){ return; }
        create_http_redirector();
    });

    server.listen( "localhost", 3000, [=]( socket_t ){
        console::log( "->", url );
    });

} catch(...) {} }

/*────────────────────────────────────────────────────────────────────────────*/

void create_tor_node_server() { try {
    if( process::env::get("CPU").empty() ){ throw ""; }

    auto cpu=string::to_uint(process::env::get("CPU"));
    auto dir=regex ::format("/tmp/tor_node_${0}",cpu);
    auto mid=regex ::format("${0}/manifest.txt" ,dir);
    auto cmd=regex ::format("tor -f ${0}",mid);

    if ( !fs::exists_file( mid ) ){ throw ""; }
    auto pid=popen::async( cmd );

} catch(...) {

    auto dir=string_t( "/tmp/tor_node_main" );
    auto mid=regex::format("${0}/manifest.txt" ,dir);
    auto cmd=regex::format("tor -f ${0}",mid);

    if ( !fs::exists_file( mid ) ){ throw ""; }
    auto pid=popen::async( cmd );
    
} }

/*────────────────────────────────────────────────────────────────────────────*/

void create_controller_manifest(){ try {
    if( process::env::get("CPU").empty() ){ throw ""; }

    auto cpu=string::to_uint(process::env::get("CPU"));
    auto dir=regex::format("/tmp/tor_node_${0}",cpu);
    auto fid=regex::format("${0}/manifest.txt" ,dir);

    if( !fs::exists_folder(dir) ){ fs::create_folder(dir); }
    if(  fs::exists_file  (fid) ){ fs::remove_file  (fid); }

    auto tport = string::to_uint( process::env::get("TOR_PORT") )+cpu;
    auto iport = string::to_uint( process::env::get("INP_PORT") );
    auto oport = string::to_uint( process::env::get("OUT_PORT") );

    fs::writable(fid).write( regex::format(_STRING_(
       SocksPort         ${0}         \n
       PidFile           ${3}/log.pid \n
       DataDirectory     ${3}/data    \n
       HiddenServiceDir  ${3}/hidden  \n
       HiddenServicePort ${1} 127.0.0.1:${2}
    ), tport, iport, oport, dir ));

} catch(...) {

    auto dir=string_t( "/tmp/tor_node_main" );
    auto fid=regex::format("${0}/manifest.txt" ,dir);

    if( !fs::exists_folder(dir) ){ fs::create_folder(dir); }
//  if(  fs::exists_file  (fid) ){ fs::remove_file  (fid); }

    auto tport = string::to_uint( process::env::get("TOR_PORT") )+os::cpus();
    auto iport = string::to_uint( process::env::get("INP_PORT") );
    auto oport = string::to_uint( process::env::get("CLS_PORT") );

    fs::writable(fid).write( regex::format(_STRING_(
       SocksPort         ${0}         \n
       PidFile           ${3}/log.pid \n
       DataDirectory     ${3}/data    \n
       HiddenServiceDir  ${3}/hidden  \n
       HiddenServicePort ${1} 127.0.0.1:${2}
    ), tport, iport, oport, dir ));

} }

/*────────────────────────────────────────────────────────────────────────────*/

void onMain() { process::env::init( ".env" );

    if( process::is_parent() ){
        create_controller_manifest();
        create_tor_node_server();
        create_load_balance();
        show_main_url();
    } else {
        create_controller_manifest();
        create_tor_node_server();
        create_http_redirector();
    }

}

/*────────────────────────────────────────────────────────────────────────────*/
