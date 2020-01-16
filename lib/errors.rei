exception Missing_env_var(string);
exception Incorrect_destination_path(string);
exception Incorrect_template_name(string);
exception Config_file_syntax_error;
exception Current_directory_not_a_spin_project;
exception Generator_does_not_exist(string);
exception Cannot_parse_template_file(string);
exception Generator_files_already_exist(string);

let handle_errors: (unit => 'a) => 'a;

type error = {
  doc: string,
  exit_code: int,
};

let all: unit => list(error);
