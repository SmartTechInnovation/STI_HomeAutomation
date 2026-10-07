#include "application.h"

#include "src/Components/Logger/logger.h"
#include "src/Components/BlockManager/blockmanager.h"
#include "src/Components/PeripheryManager/peripherymanager.h"
#include "src/Components/ProjectManager/projectmanager.h"

Application_class::Application_class() {

}

void Application_class::begin(){
    S_Logger.begin();
    S_BlockManager.begin();
    S_ProjectManager.begin();
    S_PeripheryManager.begin();
}