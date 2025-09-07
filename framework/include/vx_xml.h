/**
 * @file vx_xml.h
 * @brief XML import/export API
 * @version 0.1
 * @date 2025-08-10
 *
 * @copyright Copyright (c) 2025 Edge AI, LLC. All rights reserved.
 *
 */
#ifndef VX_XML_H
#define VX_XML_H

#include "vx_internal.h"

/*!
 * @defgroup group_int_xml XML Import/Export API
 * @ingroup group_internal
 * @brief The XML import/export API
 */
namespace coreflow
{
namespace xml
{
class Import
{
public:
    Import() = delete;
    Import(const Import &) = delete;
    Import(Import &&) = delete;
    Import &operator=(const Import &) = delete;
    Import &operator=(Import &&) = delete;
    ~Import() = delete;

    /**
     * @brief Import from XML
     *
     * @param context The context
     * @param filepath The path to the XML file
     * @return std::vector<vx_reference> The imported references
     * @ingroup group_int_xml
     */
    static std::vector<vx_graph> importFromXML(vx_context context, std::string filepath)
    {
        std::vector<vx_graph> graphs;
        vx_status status = VX_SUCCESS;
        vx_size num_refs = 0;
        vx_size num_kernels = 0;
        vx_size num_refs_added = 0;
        vx_size num_kernels_added = 0;
        vx_size num_nonkern_added = 0;
        vx_size count = 0;
        vx_size num_refs_orig = context->numReferences();
        vx_size num_kernels_orig = context->numUniqueKernels();

        auto import = vxImportFromXML(context, const_cast<vx_char *>(filepath.c_str()));
        if (import && (status = Error::getStatus(import)) == VX_SUCCESS)
        {
            num_refs = context->numReferences();
            num_kernels = context->numUniqueKernels();
            num_refs_added = num_refs - num_refs_orig;
            num_kernels_added = num_kernels - num_kernels_orig;
            num_nonkern_added = num_refs_added - num_kernels_added;

            if ((status == VX_SUCCESS) && (count = import->numRefs()) > 0)
            {
                /* Count returns the number of references explicitly found in the xml file and
                 * returned in the refs array. num_nonkern_added can be greater than count because
                 * of the following 2 reasons:
                 *    1. if there are any non-unique kernels added from a library, this will
                 * increase the num_refs count, but not the count of the number of unique kernels
                 *    2. if there are any virtual pyramids which do not have image references in the
                 * xml file, there will be added references for each level of the pyramid, but these
                 * will not be listed in the refs array
                 */
                if (num_nonkern_added < count)
                {
                    printf("ODD? Num nonkernel Refs Added = %u and Count = %u\n", num_nonkern_added,
                           count);
                    status = VX_FAILURE;
                }
            }

            /* Now that the objects are imported, add all graphs to return list */
            if (status == VX_SUCCESS)
            {
                for (vx_uint32 i = 0; i < count; i++)
                {
                    auto ref = import->getReferenceByIndex(i);
                    if (ref->dataType() == VX_TYPE_GRAPH)
                    {
                        graphs.push_back(static_cast<vx_graph>(ref));
                    }
                }
            }
        }

        return graphs;
    }
};

class Export
{
public:
    Export() = delete;
    Export(const Export &) = delete;
    Export(Export &&) = delete;
    Export &operator=(const Export &) = delete;
    Export &operator=(Export &&) = delete;
    ~Export() = delete;

    /**
     * @brief Export to XML
     *
     * @param context The context
     * @param filepath The path to the XML file
     * @return vx_status The status of the export
     * @ingroup group_int_xml
     */
    static vx_status exportToXML(vx_context context, std::string filepath)
    {
        return vxExportToXML(context, const_cast<vx_char *>(filepath.c_str()));
    }
};

}  // namespace xml
}  // namespace coreflow

#endif /* VX_XML_H */