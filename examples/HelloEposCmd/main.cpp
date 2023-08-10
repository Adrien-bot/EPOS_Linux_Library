#include "HelloEposCmd.cpp"

int main(int argc, char** argv)
{
	int lResult = MMC_FAILED;
	unsigned int ulErrorCode = 0;

	PrintHeader();

	SetDefaultParameters();

	if((lResult = ParseArguments(argc, argv))!=MMC_SUCCESS)
	{
		return lResult;
	}

	PrintSettings();

	switch(g_eAppMode)
	{
		case AM_DEMO:
		{
			if((lResult = OpenDevice(&ulErrorCode))!=MMC_SUCCESS)
			{
				LogError("OpenDevice", lResult, ulErrorCode);
				return lResult;
			}

			if((lResult = PrepareDemo(&ulErrorCode))!=MMC_SUCCESS)
			{
				LogError("PrepareDemo", lResult, ulErrorCode);
				return lResult;
			}
            // Here we change the code to input the target velocity every time the tension sensor reads a new value
			if((lResult = Demo(&ulErrorCode))!=MMC_SUCCESS)
			{
				LogError("Demo", lResult, ulErrorCode);
				return lResult;
			}

			if((lResult = CloseDevice(&ulErrorCode))!=MMC_SUCCESS)
			{
				LogError("CloseDevice", lResult, ulErrorCode);
				return lResult;
			}
		} break;
		case AM_INTERFACE_LIST:
			PrintAvailableInterfaces();
			break;
		case AM_PROTOCOL_LIST:
			PrintAvailableProtocols();
			break;
		case AM_VERSION_INFO:
		{
			if((lResult = OpenDevice(&ulErrorCode))!=MMC_SUCCESS)
			{
				LogError("OpenDevice", lResult, ulErrorCode);
				return lResult;
			}

			if((lResult = PrintDeviceVersion()) != MMC_SUCCESS)
		    {
				LogError("PrintDeviceVersion", lResult, ulErrorCode);
				return lResult;
		    }

			if((lResult = CloseDevice(&ulErrorCode))!=MMC_SUCCESS)
			{
				LogError("CloseDevice", lResult, ulErrorCode);
				return lResult;
			}
		} break;
		case AM_UNKNOWN:
			printf("unknown option\n");
			break;
	}

	return lResult;
}