macro( build_libmpeg2 )

if( CMAKE_CROSSCOMPILING )
    string( REGEX MATCH "([-A-Za-z0-9\\._]+)-(gcc|cc)$" RESULT ${CMAKE_C_COMPILER} )
    string( REGEX REPLACE "-(gcc|cc)$" "" RESULT ${RESULT} )
    set( CONFIGURE_ARGS "--host=${RESULT}" )
endif()

externalproject_add( libmpeg2
	PREFIX ${CMAKE_CURRENT_BINARY_DIR}/3rdparty
	URL ../../../src/3rdparty/libmpeg2/libmpeg2-master.tgz
	URL_HASH SHA256=e42d18a28480a426af8e4bf764c11a1c2efa3c5d21db878fb22c6cb1090c1245
	CONFIGURE_COMMAND autoreconf -f -i && <SOURCE_DIR>/configure ${CONFIGURE_ARGS} --prefix=${CMAKE_CURRENT_BINARY_DIR}/3rdparty --disable-shared --enable-static --disable-sdl
	BUILD_IN_SOURCE 1
	BUILD_COMMAND make V=0
	INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/3rdparty
	INSTALL_COMMAND make install
)

set( MPEG2_INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR}/3rdparty/include/mpeg2dec )
set( MPEG2_LIBRARIES ${CMAKE_CURRENT_BINARY_DIR}/3rdparty/lib/libmpeg2.a )
set( MPEG2_FOUND ON )

endmacro( build_libmpeg2 )
