########################################################################################
##	Supercollider_FOUND - Found Supercollider
##  Supercollider_DIR - Install folder of Supercollider with sclang application
########################################################################################

message(STATUS "")
message(STATUS "Supercollider module init...")

set(Supercollider_FOUND FALSE)

if(WIN32)
	message(STATUS "\t- Register searching init")
	set(reg_scpath "HKEY_CURRENT_USER\\Software\\SuperCollider")
	set(reg_scversion "3.9.0")
	set(reg_sckey "")
	get_filename_component(reg_value "[${reg_scpath}\\${reg_scversion};${reg_sckey}]" ABSOLUTE)	
	if(reg_value STREQUAL "/registry")
		set(reg_value "-NOTFOUND")
		message (STATUS "\t- search in register not found installed Supercollider")
	else(reg_value STREQUAL "/registry")
		message (STATUS "\t- search in register found installed Supercollider at path : " ${reg_value})
	endif(reg_value STREQUAL "/registry")	
endif(WIN32)

find_program (Supercollider_DIR "sclang"
	PATHS 
		${reg_value}
		"C:/Program Files/SuperCollider"
		"C:/Program Files/SuperCollider-3.9.0"
		"C:/Program Files/SuperCollider-3.8.0"
)

if(Supercollider_DIR)
	set(Supercollider_FOUND TRUE)
	message(STATUS "\t- Supercollider_FOUND: " ${Supercollider_FOUND})
	message(STATUS "\t- Supercollider_DIR: " ${Supercollider_DIR})
else(Supercollider_DIR)
	message(STATUS "\t- Supercollider not found")
endif(Supercollider_DIR)

message(STATUS "Supercollider module done...")
message(STATUS "")

