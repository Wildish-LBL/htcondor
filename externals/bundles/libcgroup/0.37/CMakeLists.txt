 ###############################################################
 #
 # Copyright (C) 1990-2010, Redhat.
 #
 # Licensed under the Apache License, Version 2.0 (the "License"); you
 # may not use this file except in compliance with the License.  You may
 # obtain a copy of the License at
 #
 #    http://www.apache.org/licenses/LICENSE-2.0
 #
 # Unless required by applicable law or agreed to in writing, software
 # distributed under the License is distributed on an "AS IS" BASIS,
 # WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 # See the License for the specific language governing permissions and
 # limitations under the License.
 #
 ###############################################################

if (LINUX)
	option(WITH_LIBCGROUP "Compiling with support for libcgroup" ON)
endif()

if (WITH_LIBCGROUP)

	if ( NOT PROPER )

		condor_pre_external( LIBCGROUP libcgroup-0.37 "lib;include" "lib/libcgroup.a")

		ExternalProject_Add( libcgroup
				    #-- Download Step ----------
				    DOWNLOAD_COMMAND wget -N http://cdnetworks-us-1.dl.sourceforge.net/project/libcg/libcgroup/v0.37/${LIBCGROUP_VER}.tar.bz2 ${CMD_TERM}
				    DOWNLOAD_DIR ${LIBCGROUP_STAGE}/dl
				    URL http://cdnetworks-us-1.dl.sourceforge.net/project/libcg/libcgroup/v0.37/${LIBCGROUP_VER}.tar.bz2
				    #--Configure step ----------
				    CONFIGURE_COMMAND cd ${LIBCGROUP_VER} ${CMD_TERM} ./configure --prefix=${LIBCGROUP_INSTALL_LOC} --enable-static=yes --disable-dependency-tracking
				    #--Build Step ----------
				    BUILD_COMMAND cd ${LIBCGROUP_VER} ${CMD_TERM} make
				    BUILD_IN_SOURCE 1
				    #--install Step ----------
				    INSTALL_DIR ${LIBCGROUP_INSTALL_LOC}
				    INSTALL_COMMAND cd ${LIBCGROUP_VER} ${CMD_TERM} make install )

		# Set the target dependencies which the rest of condor depends on.
		set(LIBCGROUP_FOUND "${LIBCGROUP_INSTALL_LOC}/lib/libcgroup.a")

		#set ( LIBCGROUP_SHARE "${EXTERNAL_BUILD_PREFIX}/${LIBCGROUP_VER}/src/libcgroup/${LIBCGROUP_VER}/libcgroup" )

		condor_post_external( libcgroup include OFF )

	else( NOT PROPER )

		find_multiple( "cgroup" LIBCGROUP_FOUND)

	endif( NOT PROPER )

endif()
	
if (LIBCGROUP_FOUND)
	set( LIBCGROUP_FOUND ${LIBCGROUP_FOUND} PARENT_SCOPE )
	set( HAVE_EXT_LIBCGROUP ON PARENT_SCOPE )
	message ( STATUS "external configured (LIBCGROUP_FOUND=${LIBCGROUP_FOUND})" )
else()
	message (STATUS "external skipped (libcgroup)")
endif()