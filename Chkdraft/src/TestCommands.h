#ifndef TESTCOMMANDS_H
#define TESTCOMMANDS_H
#include "CommonFiles\CommonFiles.h"
#include "GenericCommand.h"
#include "CommandTypes.h"

class ExampleCommand;
typedef std::shared_ptr<ExampleCommand> ExampleCommandPtr;

class ExampleCommand : public GenericCommand
{
    public:
        ExampleCommand(const std::string &addition);
        virtual ~ExampleCommand();
        static ExampleCommandPtr C(const std::string &addition);

    protected:
        virtual void Do();
        virtual void Undo();

    private:
        std::string addition;
        static std::string text;
};

#endif