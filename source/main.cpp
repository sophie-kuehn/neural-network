#include <iostream>
#include "snn.cpp"
#include "mnist.cpp"
#include "sclt.cpp"
#include "app.cpp"
#include "sts.cpp"

int main(int argc, char **argv)
{
    auto app = new SNN::CliApp;
    return app->main(argc, argv);
};
