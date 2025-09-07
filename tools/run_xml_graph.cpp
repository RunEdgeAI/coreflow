/**
 * @file run_xml_graph.cpp
 * @brief CoreFlow Graph Runner
 * @version 0.1
 * @date 2025-08-11
 *
 * @copyright Copyright (c) 2025 EdgeAI, LLC. All rights reserved.
 * @ingroup group_corevx_ext
 */
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

#include <VX/vx.h>
#include <VX/vx_compatibility.h>
#include <VX/vx_khr_xml.h>

class XMLGraphRunner
{
    public:
        XMLGraphRunner(const std::string& filename)
        {
            // Create context
            context_ = vxCreateContext();
            if (vxGetStatus((vx_reference)context_) != VX_SUCCESS)
            {
                throw std::runtime_error("Failed to create OpenVX context");
            }

            // Import XML
            import_ = vxImportFromXML(context_, const_cast<char*>(filename.c_str()));
            if (vxGetStatus((vx_reference)import_) != VX_SUCCESS)
            {
                vxReleaseContext(&context_);
                throw std::runtime_error("Failed to import XML file: " + filename);
            }
        }

        ~XMLGraphRunner()
        {
            if (import_) vxReleaseImport(&import_);
            if (context_) vxReleaseContext(&context_);
        }

        void runAllGraphs()
        {
            vx_uint32 count = 0;
            if (vxQueryImport(import_, VX_IMPORT_ATTRIBUTE_COUNT, &count, sizeof(count)) !=
                VX_SUCCESS)
            {
                throw std::runtime_error("Failed to query import count");
            }

            std::cout << "Running all graphs in XML..." << std::endl;
            for (vx_uint32 i = 0; i < count; i++)
            {
                vx_reference ref = vxGetImportReferenceByIndex(import_, i);
                if (ref)
                {
                    vx_enum type;
                    if (vxQueryReference(ref, VX_REF_ATTRIBUTE_TYPE, &type, sizeof(type)) ==
                        VX_SUCCESS)
                    {
                        if (type == VX_TYPE_GRAPH)
                        {
                            runGraph((vx_graph)ref);
                        }
                    }
                    vxReleaseReference(&ref);
                }
            }
        }

        void runGraphByName(const std::string& name)
        {
            vx_reference ref = vxGetImportReferenceByName(import_, name.c_str());
            if (!ref)
            {
                throw std::runtime_error("Graph '" + name + "' not found in XML");
            }

            vx_enum type;
            if (vxQueryReference(ref, VX_REF_ATTRIBUTE_TYPE, &type, sizeof(type)) == VX_SUCCESS)
            {
                if (type == VX_TYPE_GRAPH)
                {
                    runGraph((vx_graph)ref, name);
                }
                else
                {
                    vxReleaseReference(&ref);
                    throw std::runtime_error("Reference '" + name + "' is not a graph");
                }
            }
            vxReleaseReference(&ref);
        }

    private:
        void runGraph(vx_graph graph, const std::string& name = "")
        {
            vx_perf_t perf;
            vx_status status = vxProcessGraph(graph);

            if (status == VX_SUCCESS)
            {
                vxQueryGraph(graph, VX_GRAPH_PERFORMANCE, &perf, sizeof(perf));
                if (name.empty())
                {
                    std::cout << "Graph " << (vx_size)graph << " performance metrics:" << std::endl
                              << "  begin time (ns): " << perf.beg << std::endl
                              << "  end time (ns): " << perf.end << std::endl
                              << "  temp time (ns): " << perf.tmp << std::endl
                              << "  sum time (ns): " << perf.sum << std::endl
                              << "  num runs: " << perf.num << std::endl
                              << "  avg time (ns): " << perf.avg << std::endl
                              << "  min time (ns): " << perf.min << std::endl
                              << "  max time (ns): " << perf.max << std::endl;
                }
                else
                {
                    std::cout << "Graph '" << name << "' performance metrics:" << std::endl
                              << "  begin time (ns): " << perf.beg << std::endl
                              << "  end time (ns): " << perf.end << std::endl
                              << "  temp time (ns): " << perf.tmp << std::endl
                              << "  sum time (ns): " << perf.sum << std::endl
                              << "  num runs: " << perf.num << std::endl
                              << "  avg time (ns): " << perf.avg << std::endl
                              << "  min time (ns): " << perf.min << std::endl
                              << "  max time (ns): " << perf.max << std::endl;
                }
            }
            else
            {
                if (name.empty())
                {
                    std::cerr << "Failed to process graph " << (vx_size)graph << std::endl;
                }
                else
                {
                    std::cerr << "Failed to process graph '" << name << "'" << std::endl;
                }
                throw std::runtime_error("Graph processing failed");
            }
        }

        vx_context context_ = nullptr;
        vx_import import_ = nullptr;
};

void printUsage(const char* program)
{
    std::cout << "Usage: " << program << " <xml_file> [graph_name]" << std::endl;
    std::cout << "  xml_file: Path to the XML file containing graph definition" << std::endl;
    std::cout << "  graph_name: (Optional) Name of specific graph to run. If not provided, runs "
                 "all graphs"
              << std::endl;
}

int main(int argc, char* argv[])
{
    try
    {
        if (argc < 2 || argc > 3)
        {
            printUsage(argv[0]);
            return -1;
        }

        const std::string xmlFile = argv[1];
        XMLGraphRunner runner(xmlFile);

        if (argc == 3)
        {
            runner.runGraphByName(argv[2]);
        }
        else
        {
            runner.runAllGraphs();
        }

        return 0;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }
}