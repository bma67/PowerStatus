#include <list>
#include <iostream>
#include <windows.h>

enum ePowerState
{
    POWERSTATE_UNKNOWN,
    POWERSTATE_SYSTEM_POWER_UNKNOWN,
    POWERSTATE_SYSTEM_ON_POWER_SOURCE,
    POWERSTATE_SYSTEM_ON_BATTERY_SOURCE,
    POWERSTATE_BATTERY_NO_INFO,
    POWERSTATE_BATTERY_NONE,
    POWERSTATE_BATTERY_CHARGING,
    POWERSTATE_BATTERY_CRITICAL,
    POWERSTATE_BATTERY_LOW,
    POWERSTATE_BATTERY_CHARGED
};

std::list<ePowerState> getPowerStatus( int& lifeTime, int& lifePercent )
{
    std::list<ePowerState> retVal;
    
    SYSTEM_POWER_STATUS sps;

    lifeTime = 0;
    lifePercent = 0;

    if (!GetSystemPowerStatus(&sps))
    {
        retVal.push_back(POWERSTATE_UNKNOWN);
    }
    else
    {
        switch (sps.ACLineStatus)
        {
            case 0:
                retVal.push_back(POWERSTATE_SYSTEM_ON_BATTERY_SOURCE);
                break;
            case 1:
                retVal.push_back(POWERSTATE_SYSTEM_ON_POWER_SOURCE);
                break;
            case 255:
                retVal.push_back(POWERSTATE_SYSTEM_POWER_UNKNOWN);
                break;
        }

        if (sps.BatteryFlag == 0xFF)
        {
            retVal.push_back(POWERSTATE_BATTERY_NO_INFO);
        }
        else
        {
            if (sps.BatteryFlag & 0x80)
            {
                retVal.push_back(POWERSTATE_BATTERY_NONE);
            }
            if (sps.BatteryFlag & 0x08)
            {
                retVal.push_back(POWERSTATE_BATTERY_CHARGING);
            }
            if (sps.BatteryFlag & 0x04)
            {
                retVal.push_back(POWERSTATE_BATTERY_CRITICAL);
            }
            if (sps.BatteryFlag & 0x02)
            {
                retVal.push_back(POWERSTATE_BATTERY_LOW);
            }
            if (sps.BatteryFlag & 0x01)
            {
                retVal.push_back(POWERSTATE_BATTERY_CHARGED);
            }
        }

        lifeTime = sps.BatteryLifeTime == 255 ? 100 : sps.BatteryLifeTime;
        lifePercent = sps.BatteryLifePercent;
    }

    return retVal;
}

const char* sCurrentTime()
{
    static char szCurrentTime[80];

    SYSTEMTIME SystemTime;
    GetLocalTime(&SystemTime);

    sprintf_s(szCurrentTime,
        "%04d-%02d-%02d %02d:%02d:%02d.%03d",
        SystemTime.wYear, SystemTime.wMonth, SystemTime.wDay,
        SystemTime.wHour, SystemTime.wMinute, SystemTime.wSecond, SystemTime.wMilliseconds);

    return szCurrentTime;
}

void display_with_battery_details(const char* sTitle, int lifeTime, int lifePercent)
{
    std::cout << sTitle << " Life Time=" << lifeTime << " Life Percent=" << lifePercent << std::endl;
}

int main()
{
    std::cout << "Hello World!\n";

    int lifeTime, lifeTime_previous = 0;
    int lifePercent, lifePercent_previous = 0;
    std::list<ePowerState> psList_previous;

    do
    {
        std::list<ePowerState> psList = getPowerStatus(lifeTime, lifePercent);

        if ( (psList      != psList_previous     ) ||
             (lifeTime    != lifeTime_previous   ) ||
             (lifePercent != lifePercent_previous)   )
        {
            std::cout << "******** Power State has changed (" << sCurrentTime() << ") ********\n";

            for (auto ps = psList.begin() ; ps != psList.end() ; ps++)
            {
                switch (*ps)
                {
                case POWERSTATE_UNKNOWN:
                    std::cout << "Power Status unknown.\n";
                    break;

                case POWERSTATE_SYSTEM_POWER_UNKNOWN:
                    std::cout << "System power unknown.\n";
                    break;

                case POWERSTATE_SYSTEM_ON_BATTERY_SOURCE:
                    display_with_battery_details( "System is on battery.", lifeTime, lifePercent );
                    break;

                case POWERSTATE_SYSTEM_ON_POWER_SOURCE:
                    std::cout << "System is on AC power.\n";
                    break;

                case POWERSTATE_BATTERY_NO_INFO:
                    std::cout << "No battery info.\n";
                    break;

                case POWERSTATE_BATTERY_NONE:
                    std::cout << "System has no battery.\n";
                    break;

                case POWERSTATE_BATTERY_CHARGING:
                    display_with_battery_details("Battery is charging...", lifeTime, lifePercent);
                    break;

                case POWERSTATE_BATTERY_CRITICAL:
                    display_with_battery_details("Battery is critical!", lifeTime, lifePercent);
                    break;

                case POWERSTATE_BATTERY_LOW:
                    display_with_battery_details("Battery is low!", lifeTime, lifePercent);
                    break;

                case POWERSTATE_BATTERY_CHARGED:
                    std::cout << "Battery is charged.\n";
                    break;

                default:
                    break;
                }
            }

            psList_previous      = psList;
            lifeTime_previous    = lifeTime;
            lifePercent_previous = lifePercent;
        }

        Sleep(1000);

    } while (1);
}
