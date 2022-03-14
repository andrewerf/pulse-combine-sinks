#include <iostream>
#include <set>
#include <vector>
#include <memory>
#include <cstring>
#include <sstream>
#include <numeric>
#include <pulse/error.h>
#include <pulse/mainloop.h>
#include <pulse/context.h>
#include <pulse/introspect.h>


struct
{
    std::set<std::string> names;
    std::string other;
} programOptions;


void exitWithMsg(int err, std::string message)
{
    std::cout << message << std::endl;
    exit(err);
}


void onModuleLoaded(pa_context*, uint32_t, void*)
{
    exit(0);
}

pa_operation* combineSinks(pa_context* context, const std::vector<std::string> &fullNames)
{
    std::stringstream slavesString;
    slavesString << "slaves=";
    for(size_t i = 0; i < fullNames.size(); ++i)
        slavesString << fullNames[i] << ( ( i != fullNames.size() - 1 ) ? "," : "" );

    std::string opts = slavesString.str() + programOptions.other;

    return pa_context_load_module(context,
                                  "module-combine-sink",
                                  opts.c_str(),
                                  onModuleLoaded, nullptr);
}

void sinkFound(pa_context* context, const pa_sink_info* info, int eol, void* userData)
{
    auto fullNames = static_cast<std::vector<std::string>*>(userData);

    if(eol > 0)
    {
        combineSinks(context, *fullNames);
        delete fullNames;
        return;
    }

    if(strstr(info->driver, "bluez"))
    {
        // Bluetooth device
        const char* description = pa_proplist_gets(info->proplist, "device.description");
        if(programOptions.names.contains(description))
            fullNames->push_back(info->name);
    }
    if(strstr(info->driver, "alsa"))
    {
        // ALSA device
        const char* alsaName = pa_proplist_gets(info->proplist, "alsa.name");
        if(programOptions.names.contains(alsaName))
            fullNames->push_back(info->name);
    }
}

void onStateChanged(pa_context* context, void*)
{
    switch (pa_context_get_state(context)) {
        case PA_CONTEXT_FAILED: {
            auto err = pa_context_errno(context);
            exitWithMsg(err, pa_strerror(err));
        }
        case PA_CONTEXT_READY: {
            auto *fullNames = new std::vector<std::string>;
            pa_context_get_sink_info_list(context, sinkFound, fullNames);
        }
    };
}

int main(int argc, char *argv[])
{
    const auto usageString = "Usage: " + std::string(argv[0]) + " sink_1 sink_2 ... sink_n [-o \"other options\"]";
    if(argc <= 1)
        exitWithMsg(-1, usageString);


    for(int i = 1; i < argc; ++i)
    {
        if(strcmp(argv[i], "-o") == 0 && i + 1 < argc)
            programOptions.other = argv[++i];
        else
            programOptions.names.insert(argv[i]);
    }

    std::shared_ptr<pa_mainloop> mainloop{pa_mainloop_new(), pa_mainloop_free};
    pa_mainloop_api* mainloopApi = pa_mainloop_get_api(mainloop.get());
    std::shared_ptr<pa_context> context{pa_context_new_with_proplist(mainloopApi, "main context", pa_proplist_new()),
                                        std::identity()};

    pa_context_set_state_callback(context.get(), onStateChanged, nullptr);
    pa_context_connect(context.get(), nullptr, PA_CONTEXT_NOFLAGS, nullptr);

    std::cout << pa_mainloop_run(mainloop.get(), nullptr) << std::endl;
}
