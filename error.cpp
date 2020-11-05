#include <windows.h>
#include <stdio.h>

#include <vfwmsgs.h>

void GetErrorDesc(HRESULT hr, char *name, char *desc){

	const char *format1 = "%s";
	const char *format2 = "%s";

	switch(hr){
		
//---- "EIGENE" (auch aus winerror.h)
	case ERROR_FILE_NOT_FOUND:
		sprintf_s(name, 64, format1, "ERROR_FILE_NOT_FOUND");
		sprintf_s(desc, 128, format2, "The system cannot find the file specified.");
		break;
//------------

	case S_OK:
		sprintf_s(name, 64, format1, "S_OK");
		sprintf_s(desc, 128, format2, "NO ERROR");
		break;
		
	case S_FALSE:
		sprintf_s(name, 64, format1, "S_FALSE");
		sprintf_s(desc, 128, format2, "NO ERROR");
		break;
		
	case E_UNEXPECTED:
		sprintf_s(name, 64, format1, "E_UNEXPECTED");
		sprintf_s(desc, 128, format2, "Catastrophic failure");
		break;
		
	case E_NOTIMPL:
		sprintf_s(name, 64, format1, "E_NOTIMPL");
		sprintf_s(desc, 128, format2, "Not implemented");
		break;
		
	case E_OUTOFMEMORY:
		sprintf_s(name, 64, format1, "E_OUTOFMEMORY");
		sprintf_s(desc, 128, format2, "Ran out of memory");
		break;
		
	case E_INVALIDARG:
		sprintf_s(name, 64, format1, "E_INVALIDARG");
		sprintf_s(desc, 128, format2, "One or more arguments are invalid");
		break;
		
	case E_NOINTERFACE:
		sprintf_s(name, 64, format1, "E_NOINTERFACE");
		sprintf_s(desc, 128, format2, "No such interface supported");
		break;
		
	case E_POINTER:
		sprintf_s(name, 64, format1, "E_POINTER");
		sprintf_s(desc, 128, format2, "Invalid pointer");
		break;
		
	case E_HANDLE:
		sprintf_s(name, 64, format1, "E_HANDLE");
		sprintf_s(desc, 128, format2, "Invalid handle");
		break;
		
	case E_ABORT:
		sprintf_s(name, 64, format1, "E_ABORT");
		sprintf_s(desc, 128, format2, "Operation aborted");
		break;
		
	case E_FAIL:
		sprintf_s(name, 64, format1, "E_FAIL");
		sprintf_s(desc, 128, format2, "Unspecified error");
		break;
		
	case E_ACCESSDENIED:
		sprintf_s(name, 64, format1, "E_ACCESSDENIED");
		sprintf_s(desc, 128, format2, "General access denied error");
		break;
		
	case E_PENDING:
		sprintf_s(name, 64, format1, "E_PENDING");
		sprintf_s(desc, 128, format2, "The data necessary to complete this operation is not yet available.");
		break;
		
		
	//NEW
	case VFW_S_PARTIAL_RENDER:
		sprintf_s(name, 64, format1, "VFW_S_PARTIAL_RENDER");
		sprintf_s(desc, 128, format2, "Partial success; some of the streams from this pin use an unsupported format.");
		break;
	case VFW_E_CANNOT_CONNECT:
		sprintf_s(name, 64, format1, "VFW_E_CANNOT_CONNECT");
		sprintf_s(desc, 128, format2, "No combination of intermediate filters could be found to make the connection.");
		break;
	case VFW_E_NOT_IN_GRAPH:
		sprintf_s(name, 64, format1, "VFW_E_CANNOT_CONNECT");
		sprintf_s(desc, 128, format2, "At least one of the filters is not in the filter graph.");
		break;
	case VFW_E_CANNOT_RENDER:
		sprintf_s(name, 64, format1, "VFW_E_CANNOT_RENDER");
		sprintf_s(desc, 128, format2, "No combination of filters could be found to render the stream.");
				
//	case CLASS_E_CLASSNOTAVAILABLE: 0x80040111 -2147221231
//		sprintf_s(name, 64, format1, "CLASS_E_CLASSNOTAVAILABLE");
//		sprintf_s(desc, 128, format2, "ClassFactory cannot supply requested class.");
	
	case CO_E_INIT_TLS:
		sprintf_s(name, 64, format1, "CO_E_INIT_TLS");
		sprintf_s(desc, 128, format2, "Thread local storage failure");
		break;
		
	case CO_E_INIT_SHARED_ALLOCATOR:
		sprintf_s(name, 64, format1, "CO_E_INIT_SHARED_ALLOCATOR");
		sprintf_s(desc, 128, format2, "Get shared memory allocator failure");
		break;
		
	case CO_E_INIT_MEMORY_ALLOCATOR:
		sprintf_s(name, 64, format1, "CO_E_INIT_MEMORY_ALLOCATOR");
		sprintf_s(desc, 128, format2, "Get memory allocator failure");
		break;
		
	case CO_E_INIT_CLASS_CACHE:
		sprintf_s(name, 64, format1, "CO_E_INIT_CLASS_CACHE");
		sprintf_s(desc, 128, format2, "Unable to initialize class cache");
		break;
		
	case CO_E_INIT_RPC_CHANNEL:
		sprintf_s(name, 64, format1, "CO_E_INIT_RPC_CHANNEL");
		sprintf_s(desc, 128, format2, "Unable to initialize RPC services");
		break;
		
	case CO_E_INIT_TLS_SET_CHANNEL_CONTROL:
		sprintf_s(name, 64, format1, "CO_E_INIT_TLS_SET_CHANNEL_CONTROL");
		sprintf_s(desc, 128, format2, "Cannot set thread local storage channel control");
		break;
		
	case CO_E_INIT_TLS_CHANNEL_CONTROL:
		sprintf_s(name, 64, format1, "CO_E_INIT_TLS_CHANNEL_CONTROL");
		sprintf_s(desc, 128, format2, "Could not allocate thread local storage channel control");
		break;
		
	case CO_E_INIT_UNACCEPTED_USER_ALLOCATOR:
		sprintf_s(name, 64, format1, "CO_E_INIT_UNACCEPTED_USER_ALLOCATOR");
		sprintf_s(desc, 128, format2, "The user supplied memory allocator is unacceptable");
		break;
		
	case CO_E_INIT_SCM_MUTEX_EXISTS:
		sprintf_s(name, 64, format1, "CO_E_INIT_SCM_MUTEX_EXISTS");
		sprintf_s(desc, 128, format2, "The OLE service mutex already exists");
		break;
		
	case CO_E_INIT_SCM_FILE_MAPPING_EXISTS:
		sprintf_s(name, 64, format1, "CO_E_INIT_SCM_FILE_MAPPING_EXISTS");
		sprintf_s(desc, 128, format2, "The OLE service file mapping already exists");
		break;
		
	case CO_E_INIT_SCM_MAP_VIEW_OF_FILE:
		sprintf_s(name, 64, format1, "CO_E_INIT_SCM_MAP_VIEW_OF_FILE");
		sprintf_s(desc, 128, format2, "Unable to map view of file for OLE service");
		break;
		
	case CO_E_INIT_SCM_EXEC_FAILURE:
		sprintf_s(name, 64, format1, "CO_E_INIT_SCM_EXEC_FAILURE");
		sprintf_s(desc, 128, format2, "Failure attempting to launch OLE service");
		break;
		
	case CO_E_INIT_ONLY_SINGLE_THREADED:
		sprintf_s(name, 64, format1, "CO_E_INIT_ONLY_SINGLE_THREADED");
		sprintf_s(desc, 128, format2, "There was an attempt to call CoInitialize a second time while single threaded");
		break;
		
	case CO_E_CANT_REMOTE:
		sprintf_s(name, 64, format1, "CO_E_CANT_REMOTE");
		sprintf_s(desc, 128, format2, "A Remote activation was necessary but was not allowed");
		break;
		
	case CO_E_BAD_SERVER_NAME:
		sprintf_s(name, 64, format1, "CO_E_BAD_SERVER_NAME");
		sprintf_s(desc, 128, format2, "A Remote activation was necessary but the server name provided was invalid");
		break;
		
	case CO_E_WRONG_SERVER_IDENTITY:
		sprintf_s(name, 64, format1, "CO_E_WRONG_SERVER_IDENTITY");
		sprintf_s(desc, 128, format2, "The class is configured to run as a security id different from the caller");
		break;
		
	case CO_E_OLE1DDE_DISABLED:
		sprintf_s(name, 64, format1, "CO_E_OLE1DDE_DISABLED");
		sprintf_s(desc, 128, format2, "Use of Ole1 services requiring DDE windows is disabled");
		break;
		
	case CO_E_RUNAS_SYNTAX:
		sprintf_s(name, 64, format1, "CO_E_RUNAS_SYNTAX");
		sprintf_s(desc, 128, format2, "A RunAs specification must be <domain name>\\<user name> or simply <user name>");
		break;
		
	case CO_E_CREATEPROCESS_FAILURE:
		sprintf_s(name, 64, format1, "CO_E_CREATEPROCESS_FAILURE");
		sprintf_s(desc, 128, format2, "The server process could not be started. The pathname may be incorrect.");
		break;
		
	case CO_E_RUNAS_CREATEPROCESS_FAILURE:
		sprintf_s(name, 64, format1, "CO_E_RUNAS_CREATEPROCESS_FAILURE");
		sprintf_s(desc, 128, format2, "The server process could not be started as the configured identity. The pathname may be incorrect or unavailable.");
		break;
		
	case CO_E_RUNAS_LOGON_FAILURE:
		sprintf_s(name, 64, format1, "CO_E_RUNAS_LOGON_FAILURE");
		sprintf_s(desc, 128, format2, "The server process could not be started because the configured identity is incorrect. Check the username and password.");
		break;
		
	case CO_E_LAUNCH_PERMSSION_DENIED:
		sprintf_s(name, 64, format1, "CO_E_LAUNCH_PERMSSION_DENIED");
		sprintf_s(desc, 128, format2, "The client is not allowed to launch this server.");
		break;
		
	case CO_E_START_SERVICE_FAILURE:
		sprintf_s(name, 64, format1, "CO_E_START_SERVICE_FAILURE");
		sprintf_s(desc, 128, format2, "The service providing this server could not be started.");
		break;
		
	case CO_E_REMOTE_COMMUNICATION_FAILURE:
		sprintf_s(name, 64, format1, "CO_E_REMOTE_COMMUNICATION_FAILURE");
		sprintf_s(desc, 128, format2, "This computer was unable to communicate with the computer providing the server.");
		break;
		
	case CO_E_SERVER_START_TIMEOUT:
		sprintf_s(name, 64, format1, "CO_E_SERVER_START_TIMEOUT");
		sprintf_s(desc, 128, format2, "The server did not respond after being launched.");
		break;
		
	case CO_E_CLSREG_INCONSISTENT:
		sprintf_s(name, 64, format1, "CO_E_CLSREG_INCONSISTENT");
		sprintf_s(desc, 128, format2, "The registration informat1ion for this server is inconsistent or incomplete.");
		break;
		
	case CO_E_IIDREG_INCONSISTENT:
		sprintf_s(name, 64, format1, "CO_E_IIDREG_INCONSISTENT");
		sprintf_s(desc, 128, format2, "The registration informat1ion for this interface is inconsistent or incomplete.");
		break;
		
	case CO_E_NOT_SUPPORTED:
		sprintf_s(name, 64, format1, "CO_E_NOT_SUPPORTED");
		sprintf_s(desc, 128, format2, "The operation attempted is not supported.");
		break;
		
	case CO_E_RELOAD_DLL:
		sprintf_s(name, 64, format1, "CO_E_RELOAD_DLL");
		sprintf_s(desc, 128, format2, "A dll must be loaded.");
		break;
		
	case CO_E_MSI_ERROR:
		sprintf_s(name, 64, format1, "CO_E_MSI_ERROR");
		sprintf_s(desc, 128, format2, "A Microsoft Software Installer error was encountered.");
		break;
		
	case CO_E_ATTEMPT_TO_CREATE_OUTSIDE_CLIENT_CONTEXT:
		sprintf_s(name, 64, format1, "CO_E_ATTEMPT_TO_CREATE_OUTSIDE_CLIENT_CONTEXT");
		sprintf_s(desc, 128, format2, "The specified activation could not occur in the client contextas specified.");
		break;
		
	case CO_E_SERVER_PAUSED:
		sprintf_s(name, 64, format1, "CO_E_SERVER_PAUSED");
		sprintf_s(desc, 128, format2, "Activations on the server are paused.");
		break;
		
	case CO_E_SERVER_NOT_PAUSED:
		sprintf_s(name, 64, format1, "CO_E_SERVER_NOT_PAUSED");
		sprintf_s(desc, 128, format2, "Activations on the server are not paused.");
		break;
		
	case CO_E_CLASS_DISABLED:
		sprintf_s(name, 64, format1, "CO_E_CLASS_DISABLED");
		sprintf_s(desc, 128, format2, "The component or application containing the component has been disabled.");
		break;
		
	case CO_E_CLRNOTAVAILABLE:
		sprintf_s(name, 64, format1, "CO_E_CLRNOTAVAILABLE");
		sprintf_s(desc, 128, format2, "The common language runtime is not available");
		break;
		
	case CO_E_ASYNC_WORK_REJECTED:
		sprintf_s(name, 64, format1, "CO_E_ASYNC_WORK_REJECTED");
		sprintf_s(desc, 128, format2, "The thread-pool rejected the submitted asynchronous work.");
		break;
		
	case CO_E_SERVER_INIT_TIMEOUT:
		sprintf_s(name, 64, format1, "CO_E_SERVER_INIT_TIMEOUT");
		sprintf_s(desc, 128, format2, "The server started, but did not finish initializing in a timely fashion.");
		break;
		
	case CO_E_NO_SECCTX_IN_ACTIVATE:
		sprintf_s(name, 64, format1, "CO_E_NO_SECCTX_IN_ACTIVATE");
		sprintf_s(desc, 128, format2, "Unable to complete the call since there is no COM+ security context inside IObjectControl.Activate.");
		break;
		
	case CO_E_TRACKER_CONFIG:
		sprintf_s(name, 64, format1, "CO_E_TRACKER_CONFIG");
		sprintf_s(desc, 128, format2, "The provided tracker configuration is invalid");
		break;
		
	case CO_E_THREADPOOL_CONFIG:
		sprintf_s(name, 64, format1, "CO_E_THREADPOOL_CONFIG");
		sprintf_s(desc, 128, format2, "The provided thread pool configuration is invalid");
		break;
		
	case CO_E_SXS_CONFIG:
		sprintf_s(name, 64, format1, "CO_E_SXS_CONFIG");
		sprintf_s(desc, 128, format2, "The provided side-by-side configuration is invalid");
		break;
		
	case CO_E_MALFORMED_SPN:
		sprintf_s(name, 64, format1, "CO_E_MALFORMED_SPN");
		sprintf_s(desc, 128, format2, "The server principal name (SPN) obtained during security negotiation is malformed.");
		break;
		
		// OLE
		
	case REGDB_E_READREGDB:
		sprintf_s(name, 64, format1, "REGDB_E_READREGDB");
		sprintf_s(desc, 128, format2, "Could not read key from registry");
		break;
		
	case REGDB_E_WRITEREGDB:
		sprintf_s(name, 64, format1, "REGDB_E_WRITEREGDB");
		sprintf_s(desc, 128, format2, "Could not write key to registry");
		break;
		
	case REGDB_E_KEYMISSING:
		sprintf_s(name, 64, format1, "REGDB_E_KEYMISSING");
		sprintf_s(desc, 128, format2, "Could not find the key in the registry");
		break;
		
	case REGDB_E_INVALIDVALUE:
		sprintf_s(name, 64, format1, "REGDB_E_INVALIDVALUE");
		sprintf_s(desc, 128, format2, "Invalid value for registry");
		break;
		
	case REGDB_E_CLASSNOTREG:
		sprintf_s(name, 64, format1, "REGDB_E_CLASSNOTREG");
		sprintf_s(desc, 128, format2, "Class not registered");
		break;
		
	case REGDB_E_IIDNOTREG:
		sprintf_s(name, 64, format1, "REGDB_E_IIDNOTREG");
		sprintf_s(desc, 128, format2, "Interface not registered");
		break;
		
	default:
		sprintf_s(name, 64, format1, "");
		sprintf_s(desc, 128, format2, "Unknown Error");
		break;
		
	}

}