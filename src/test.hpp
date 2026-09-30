// #pragma once
// template<typename T>
// using return_type_t = [:return_type_of(^^T):];
//
// template<typename T> concept class_type = std::is_class_v<T>;
// template<typename R, typename F> concept return_type_of = std::same_as<return_type_t<F>, R>;
//
// struct cond {
//     meta::info compare_to;
// };
//
// struct cond_annotation_info {
//     meta::info compare_to;
//     meta::info self;
//     meta::info compare_to_obj;
// };
//
// consteval std::vector<cond_annotation_info> find_conds_impl(meta::info M) {
//     std::vector<cond_annotation_info> conds;
//
//     for (auto &m: nonstatic_data_members_of(M, ctx)) {
//         auto t = meta::type_of(m);
//         if (meta::is_class_type(t) && meta::is_complete_type(t)) {
//             auto c = find_conds_impl(t);
//             conds.insert(conds.end(), c.begin(), c.end());
//         }
//
//         for (const auto &a: annotations_of_with_type(m, ^^cond)) {
//             auto [compare_to] = meta::extract<cond>(a);
//             conds.push_back({
//                 .compare_to = compare_to,
//                 .self = m,
//             });
//         }
//     }
//
//     return conds;
// }
//
// template<class_type T>
// consteval void traverse_fields_impl(T &v, ssize_t &offset, auto conds) {
//     static constexpr auto members = std::define_static_array(nonstatic_data_members_of(^^T, ctx));
//
//     static constexpr auto op = [] (auto &dst, const auto &src) {
//         dst += src;
//     };
//
//     template for (constexpr auto &m: members) {
//         constexpr auto t = meta::type_of(m);
//         using MT = [:t:];
//
//         // FIXME: custom types, flags, etc...
//         if constexpr (meta::is_class_type(t) && meta::is_complete_type(t)) {
//             traverse_fields_impl<T>(v.[:m:], offset, conds);
//             continue; // FIXME: remove in io
//         }
//
//         template for (constexpr auto &cond : conds)
//             if constexpr (cond.self == m)
//                 op(v.[:m:], [: cond.compare_to_obj :]);
//
//         // read...
//         if constexpr (t == ^^int)
//             v.[:m:] *= -1;
//         offset++;
//
//         template for (auto &cond : conds)
//             if (cond.compare_to == m)
//                 cond.compare_to_obj = meta::reflect_object(v.[:m:]);
//     }
// }
//
// template <class_type T>
// consteval auto traverse_fields(T &v) {
//     ssize_t offset{};
//
//     traverse_fields_impl(v, offset, find_conds(v));
//     return offset;
// }
//
//
