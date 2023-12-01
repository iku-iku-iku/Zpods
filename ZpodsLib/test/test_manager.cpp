//
// Created by code4love on 23-12-1.
//

#include <gtest/gtest.h>
#include "manager.h"

using namespace zpods;

TEST(ManagerTest, LoadPodsTest) {
    PodsManager manager;
    manager.load_pods("/Users/code4love/Downloads/zpods");
}