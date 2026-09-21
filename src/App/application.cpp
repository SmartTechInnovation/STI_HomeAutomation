#include "application.h"

#include "src/Components/BlockManager/blockmanager.h"

Application_class::Application_class() {

}

void Application_class::begin(){
    S_BlockManager.begin();
}