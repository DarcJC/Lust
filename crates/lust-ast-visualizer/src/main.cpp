#include "cxxopts/cxxopts.h"
#include "graphviz/gvc.h"
#include "lust/container/unique_ptr.hpp"
#include "lust/container/vector.hpp"
#include "lust/grammar.hpp"
#include "lust/lexer.hpp"
#include "lust/parser.hpp"
#include <functional>
#include <graphviz/cgraph.h>
#include <graphviz/gvcext.h>
#include <iostream>
#include <cstdio>
#include <sstream>

std::string read_stdin() {
    std::stringstream ss;
    char c;
    while ((c = getchar()) != EOF) {
        ss << c;
    }
    return ss.str();
}

int main(int argc, char* argv[]) {

    try {
        cxxopts::Options options("Lust-ASTVisualizer", "Visualize the AST nodes");
        options.add_options()
            ("verbose", "Verbose output")
            ("v,version", "Print version")
            ("h,help", "Print help message")
        ;
        options.add_options("input")
            ("f,file", "Read from file", cxxopts::value<std::string>())
        ;
        auto cli_args = options.parse(argc, argv);

        if (cli_args.count("version")) {
            std::cout << "Lust ASTVisualizer v1.0.0" << std::endl;
            return 0;
        } else if (cli_args.count("help")) {
            std::cout << options.help() << std::endl;
            return 0;
        }

        if (cli_args.count("file")) {
            std::freopen(cli_args["file"].as<std::string>().c_str(), "r", stdin);
        }

        std::string code = read_stdin();
        lust::lexer::TokenStream tokens = lust::lexer::ITokenizer::create(code);
        lust::UniquePtr<lust::grammar::ASTNode_Program> program = lust::grammar::IParser::create(tokens)->parse();

        {
            std::string graph_name = "AST";
            std::string root_name = "PROGRAM";
            GVC_t* gv_context = gvContext();
            Agraph_t* graph = agopen(graph_name.data(), Agdirected, nullptr);
            
            Agnode_t* root_node = agnode(graph, root_name.data(), 1);
            std::function<void(Agnode_t*, const lust::vector<const lust::grammar::IASTNode*>&)> add_nodes = [graph, &add_nodes] (Agnode_t* parent_node, const lust::vector<const lust::grammar::IASTNode*>& nodes) {
                for (const lust::grammar::IASTNode* n : nodes) {
                    if (nullptr == n) {
                        continue;
                    }
                    Agnode_t* new_node = agnode(graph, const_cast<char*>(lust::grammar::grammar_rule_to_name(n->get_type())), 1);
                    agedge(graph, parent_node, new_node, "child", 1);
                    add_nodes(new_node, n->collect_self_nodes());
                }
            };
            add_nodes(root_node, program->collect_self_nodes());

            gvLayout(gv_context, graph, "dot");
            gvRenderFilename(gv_context, graph, "png", "output.png");

            gvFreeLayout(gv_context, graph);
            agclose(graph);
            gvFreeContext(gv_context);
        }
    }
    catch (cxxopts::exceptions::parsing& err) {
        std::cerr << "Invalid argument inputs" << std::endl;
        return 1;
    }
    catch (cxxopts::exceptions::specification& err) {
    }

    return 0;
}
