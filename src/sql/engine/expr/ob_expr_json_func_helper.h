/**
 * Copyright (c) 2021 OceanBase
 * OceanBase CE is licensed under Mulan PubL v2.
 * You can use this software according to the terms and conditions of the Mulan PubL v2.
 * You may obtain a copy of Mulan PubL v2 at:
 *          http://license.coscl.org.cn/MulanPubL-2.0
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PubL v2 for more details.
 */

// This file is for define of func json expr helper
#ifndef OCEANBASE_SQL_OB_EXPR_JSON_FUNC_HELPER_H_
#define OCEANBASE_SQL_OB_EXPR_JSON_FUNC_HELPER_H_

#include "sql/engine/expr/ob_expr_util.h"
#include "share/object/ob_obj_cast.h"
#include "sql/parser/ob_item_type.h"
#include "sql/session/ob_sql_session_info.h"
#include "lib/json_type/ob_json_tree.h"
#include "lib/json_type/ob_json_base.h"
#include "lib/json_type/ob_json_parse.h"

using namespace oceanbase::common;

namespace oceanbase
{
namespace sql
{
class ObJsonExprHelper final
{
  class ObJsonPathCacheCtx : public ObExprOperatorCtx
  {
  public:
    ObJsonPathCacheCtx(common::ObIAllocator *allocator)
      : ObExprOperatorCtx(),
        path_cache_(allocator)
    {}
    virtual ~ObJsonPathCacheCtx() {}
    ObJsonPathCache *get_path_cache() { return &path_cache_; }
  private:
    ObJsonPathCache path_cache_;
  };
public:
  /*
  get json doc to JsonBase in static_typing_engine
  @param[in]  expr       the input arguments
  @param[in]  ctx        the eval context
  @param[in]  allocator  the Allocator in context
  @param[in]  index      the input arguments index
  @param[out] j_base     the pointer to JsonBase
  @param[out] is_null    the flag for null situation
  @return Returns OB_SUCCESS on success, error code otherwise.
  */
  static int get_json_doc(const ObExpr &expr, ObEvalCtx &ctx,
                          common::ObArenaAllocator &allocator,
                          uint16_t index, ObIJsonBase*& j_base,
                          bool &is_null, bool need_to_tree=true);

  /*
  get json doc to JsonBase in old_typing_engine
  @param[in]  objs       the input arguments
  @param[in]  allocator  the Allocator in context
  @param[in]  index      the input arguments index
  @param[out] j_base     the pointer to JsonBase
  @param[out] is_null    the flag for null situation
  @return Returns OB_SUCCESS on success, error code otherwise.
  */
  static int get_json_doc(const ObObj *objs, common::ObIAllocator *allocator,
                          uint16_t index, ObIJsonBase*& j_base,
                          bool &is_null, bool need_to_tree = true);

  /*
  get json value to JsonBase in static_typing_engine
  @param[in]  expr       the input arguments
  @param[in]  ctx        the eval context
  @param[in]  allocator  the Allocator in context
  @param[in]  index      the input arguments index
  @param[out] j_base     the pointer to JsonBase
  @param[in]  to_bin     expect value is binary or not
  @return Returns OB_SUCCESS on success, error code otherwise.
  */
  static int get_json_val(const ObExpr &expr, ObEvalCtx &ctx,
                          common::ObIAllocator *allocator, uint16_t index,
                          ObIJsonBase*& j_base, bool to_bin = false);

  /*
  get json value to JsonBase in old_typing_engine
  @param[in]  data       the input argument
  @param[in]  is_bool    whether input data is JsonBoolean
  @param[in]  ctx        the eval context
  @param[in]  allocator  the Allocator in context
  @param[in]  to_bin     expect value is binary or not
  @param[out] j_base     the pointer to JsonBase
  @return Returns OB_SUCCESS on success, error code otherwise.
  */
  static int get_json_val(const common::ObObj &data, ObExprCtx &ctx,
                          bool is_bool, common::ObIAllocator *allocator,
                          ObIJsonBase*& j_base, bool to_bin = false);
 
  /*
  replace json_old with json_new in json_doc
  @param[in]  json_old   the old json node to be replaced
  @param[in]  json_new   the new json node to replace
  @param[in]  json_doc   the root json node of old json node
  @return Returns OB_SUCCESS on success, error code otherwise.
  */
  static int json_base_replace(ObIJsonBase *json_old, ObIJsonBase *json_new,
                               ObIJsonBase *&json_doc);

  static int find_and_add_cache(ObJsonPathCache* path_cache, ObJsonPath*& res_path,
                                ObString& path_str, int arg_idx, bool enable_wildcard);

  static ObJsonPathCache* get_path_cache_ctx(const uint64_t& id, ObExecContext *exec_ctx);

  static int is_json_zero(const ObString& data, int& result);

  /*
  try to transfrom scalar data to jsonBase
  @param[in]  datum          the input datum
  @param[in]  type           the type of input argument
  @param[in]  allocator      the Allocator in context
  @param[in]  scale          the scale for json decimal
  @param[in]  ObTimeZoneInfo the timeZone info
  @param[out] j_base        the pointer to ObIJsonBase
  @param[in]  to_bin         whether convert to binary
  @return Returns OB_SUCCESS on success, error code otherwise.
  */
  template <typename T>
  static int transform_scalar_2jsonBase(const T &datum,
                                        ObObjType type,
                                        common::ObIAllocator *allocator,
                                        ObScale scale,
                                        const ObTimeZoneInfo *tz_info,
                                        ObIJsonBase*& j_base,
                                        bool to_bin);
  /*
  try to transfrom from type which is_convertible_to_json to jsonBase
  @param[in]  datum          the input datum
  @param[in]  type           the type of input argument
  @param[in]  allocator      the Allocator in context
  @param[in]  cs_type        the collation type
  @param[out] j_base         the pointer to jsonBase
  @param[in]  to_bin         whether convert to binary
  @param[in]  deep_copy      whether deep copy input string from datum
  @return Returns OB_SUCCESS on success, error code otherwise.
  */
  template <typename T>
  static int transform_convertible_2jsonBase(const T &datum,
                                             ObObjType type,
                                             common::ObIAllocator *allocator,
                                             ObCollationType cs_type,
                                             ObIJsonBase*& j_base,
                                             bool to_bin,
                                             bool deep_copy = false);

  static bool is_convertible_to_json(ObObjType &type);
  static int is_valid_for_json(ObExprResType* types_stack, uint32_t index, const char* func_name);
  static int is_valid_for_json(ObExprResType& type, uint32_t index, const char* func_name);
  static int is_valid_for_path(ObExprResType* types_stack, uint32_t index);
  static void set_type_for_value(ObExprResType* types_stack, uint32_t index);
  static int ensure_collation(ObObjType type, ObCollationType cs_type);
  static ObJsonInType get_json_internal_type(ObObjType type);
private:
  DISALLOW_COPY_AND_ASSIGN(ObJsonExprHelper);
};

} // sql
} // oceanbase
#endif // OCEANBASE_SQL_OB_EXPR_JSON_FUNC_HELPER_H_