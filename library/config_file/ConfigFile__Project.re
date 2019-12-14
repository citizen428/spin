open Jingoo;

[@deriving sexp]
type cst =
  | Source(string)
  | Cfg_str(string, string)
  | Cfg_int(string, int)
  | Cfg_float(string, float)
  | Cfg_list(string, list(string))
  | Cfg_bool(string, bool);

type t = {
  models: list((string, Jg_types.tvalue)),
  source: string,
};

let path = ".spin";

let t_of_cst = (~useDefaults, ~models, cst: list(cst)): t => {
  let newModels =
    ConfigFile__CstUtils.get(
      cst,
      ~f=
        fun
        | Cfg_str(name, v) => Some((name, Jg_types.Tstr(v)))
        | Cfg_int(name, v) => Some((name, Jg_types.Tint(v)))
        | Cfg_float(name, v) => Some((name, Jg_types.Tfloat(v)))
        | Cfg_list(name, v) => {
            let strs = List.map(v, ~f=s => Jg_types.Tstr(s));
            Some((name, Jg_types.Tlist(strs)));
          }
        | Cfg_bool(name, v) => Some((name, Jg_types.Tbool(v)))
        | Source(_) => None,
    );

  {
    source:
      ConfigFile__CstUtils.getUniqueExn(
        cst,
        ~f=
          fun
          | Source(v) => Some(v)
          | _ => None,
      ),
    models: newModels,
  };
};

let cst_of_t = (~models, t: t): list(cst) => {
  let prepend = (~l, a) => [a, ...l];

  let cst =
    List.fold(models, ~init=[], ~f=(acc, (name, value)) => {
      switch (value) {
      | Jg_types.Tstr(v) => Cfg_str(name, v) |> prepend(~l=acc)
      | Jg_types.Tint(v) => Cfg_int(name, v) |> prepend(~l=acc)
      | Jg_types.Tbool(v) => Cfg_bool(name, v) |> prepend(~l=acc)
      | Jg_types.Tfloat(v) => Cfg_float(name, v) |> prepend(~l=acc)
      | Jg_types.Tlist(v) =>
        let strs =
          List.fold(v, ~init=[], ~f=acc =>
            (
              fun
              | Jg_types.Tstr(v) => [v, ...acc]
              | _ => acc
            )
          );
        Cfg_list(name, strs) |> prepend(~l=acc);
      | _ => acc
      }
    });

  [Source(t.source), ...cst];
};

let save = (data: t, ~destination: string) => {
  let destination = Utils.Filename.concat(destination, path);
  let sexpString =
    cst_of_t(data, ~models=data.models)
    |> List.map(~f=cst => cst |> sexp_of_cst |> Sexp.to_string)
    |> Utils.String.join(~sep="\n");
  Stdio.Out_channel.write_all(destination, ~data=sexpString);
};
