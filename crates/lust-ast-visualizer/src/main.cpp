#include "cxxopts/cxxopts.h"
#include "graphviz/gvc.h"
#include "graphviz/cgraph.h"
#include "graphviz/gvcext.h"
#include "lust/container/unique_ptr.hpp"
#include "lust/container/vector.hpp"
#include "lust/grammar.hpp"
#include "lust/lexer.hpp"
#include "lust/parser.hpp"
#include <exception>
#include <functional>
#include <iostream>
#include <cstdio>
#include <sstream>
#include <exception>
#include <atomic>
#include <string>

std::string read_stdin() {
    std::stringstream ss;
    char c;
    while ((c = getchar()) != EOF) {
        ss << c;
    }
    return ss.str();
}

size_t get_unique_id() {
    static std::atomic<size_t> at;
    return at.fetch_add(1);
}

std::string get_unique_name(std::string_view sv) {
    std::stringstream ss;
    ss << sv << '_' << get_unique_id();
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
        options.add_options("output")
            ("o,output", "Output image path", cxxopts::value<std::string>()->default_value("output.png"))
        ;
        auto cli_args = options.parse(argc, argv);

        if (cli_args.count("version")) {
            std::cout << "Lust ASTVisualizer v" << LUST_VERSION << std::endl;
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
            GVC_t* gv_context = gvContext();
            Agraph_t* graph = agopen(get_unique_name("AST").data(), Agdirected, nullptr);
            
            Agnode_t* root_node = agnode(graph, get_unique_name("PROGRAM").data(), 1);
            std::function<void(Agnode_t*, const lust::vector<const lust::grammar::IASTNode*>&)> add_nodes = [graph, &add_nodes] (Agnode_t* parent_node, const lust::vector<const lust::grammar::IASTNode*>& nodes) {
                for (const lust::grammar::IASTNode* n : nodes) {
                    if (nullptr == n) {
                        continue;
                    }
                    Agnode_t* new_node = agnode(graph, get_unique_name(lust::grammar::grammar_rule_to_name(n->get_type())).data(), 1);
                    agedge(graph, parent_node, new_node, get_unique_name("CHILD").data(), 1);
                    add_nodes(new_node, n->collect_self_nodes());
                }
            };
            add_nodes(root_node, program->collect_self_nodes());

            gvLayout(gv_context, graph, "dot");
            gvRenderFilename(gv_context, graph, "png", cli_args["output"].as<std::string>().c_str());

            gvFreeLayout(gv_context, graph);
            agclose(graph);
            gvFreeContext(gv_context);
        }
    }
    catch (cxxopts::exceptions::parsing& err) {
        std::cerr << "Invalid argument inputs: " << err.what() << std::endl;
        return 1;
    }
    catch (std::exception& err) {
        std::cerr << "Unknown error: " << err.what() << std::endl;
        return 2;
    }

    return 0;
}
