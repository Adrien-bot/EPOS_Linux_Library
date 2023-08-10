#pragma once

#include <iostream>
#include "Definitions.h"
#include <string.h>
#include <sstream>
#include <list>

#include "HelloEposCmd.hpp"

class DevicePort
{
public:
    HANDLE p_DeviceHandle;
    unsigned short p_usNodeId;
    unsigned int *p_rlErrorCode;
    int *p_lResult;
    DevicePort(HANDLE DeviceHandle, unsigned short usNodeId, unsigned int *rlErrorCode, int *lResult) : p_DeviceHandle{DeviceHandle}, p_usNodeId{usNodeId}, p_rlErrorCode{rlErrorCode}, p_lResult{lResult}
    {
    }
    ~DevicePort()
    {
        delete p_rlErrorCode;
        delete p_lResult;
    }

    // Copy constructor -- no copying allowed!
    DevicePort(const DevicePort &a) = delete;

    // Move constructor
    // Transfer ownership
    DevicePort(DevicePort &&a) noexcept
        : p_DeviceHandle{a.p_DeviceHandle}, p_usNodeId{a.p_usNodeId}, p_rlErrorCode{a.p_rlErrorCode}, p_lResult{a.p_lResult}
    {
        a.p_rlErrorCode = nullptr;
        a.p_lResult = nullptr;
    }

    // Copy assignment -- no copying allowed!
    DevicePort &operator=(const DevicePort &a) = delete;

    // Move assignment
    // Transfer ownership 
    DevicePort &operator=(DevicePort &&a) noexcept
    {
        // Self-assignment detection
        if (&a == this)
            return *this;

        delete p_rlErrorCode;
        delete p_lResult;
        // Transfer ownership 
        p_DeviceHandle = a.p_DeviceHandle;
        p_usNodeId = a.p_usNodeId;
        p_rlErrorCode = a.p_rlErrorCode;
        a.p_rlErrorCode = nullptr;
        p_lResult = a.p_lResult;
        a.p_lResult = nullptr;
        return *this;
    }
};

void VelocityControlLoop(DevicePort &dev)
{
    std::list<long> velocityList;
    double cable_tension_desired = 10; // En newton. Deviendra le paramètre de fonction
    double K_Tens = 1;                 // gain pour transformer erreur de tension en commande de vitesse. Sera paramétré une fois la boucle construite

    // Cette boucle de controle doit s'executer pour chaque nouvelle valeur de tension lue (boucle fermée). Idéalement on aimerait en sortir dès que l'utilisateur envoie un signal ou que la capteur cesse de transmettre.
    while (true)
    {
        double cable_tension_measured = 5; // readNewSensorTension(); // Trouver moyen de la lire ici
        double tension_error = cable_tension_desired - cable_tension_measured;
        long targetvelocity = static_cast<long>(K_Tens * tension_error);
        velocityList.push_back(targetvelocity);
        std::stringstream msg;
        msg << "measured a tension of " << cable_tension_measured << ", the error woth regards to the target tension is " << tension_error << std::endl;
        msg << "move with target velocity = " << velocityList.front() << " rpm";
        LogInfo(msg.str());

        if (VCS_MoveWithVelocity(dev.p_DeviceHandle, dev.p_usNodeId, velocityList.front(), dev.p_rlErrorCode) == 0)
        {
            *(dev.p_lResult) = MMC_FAILED;
            LogError("VCS_MoveWithVelocity", *(dev.p_lResult), *(dev.p_rlErrorCode));
            break;
        }
        velocityList.pop_front();
        sleep(1); // il faudra changer ça selon la vitesse d'acquisition/d'execution
    }
}