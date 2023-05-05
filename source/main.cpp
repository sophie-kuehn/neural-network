#include "../header/app.hpp"

int main(int argc, char **argv)
{
    auto app = new SNN::CliApp;
    return app->main(argc, argv);
};
