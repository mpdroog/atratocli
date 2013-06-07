_aci()
{
        local cur prev opts opt_credentials

        cur="${COMP_WORDS[COMP_CWORD]}"
        prev="${COMP_WORDS[COMP_CWORD-1]}"

        opts="db credential --help --verbose --version"
        opt_credentials="add search"
        opt_dbs="init"

        if [[ ${prev} == 'credential' ]]; then
                COMPREPLY=( $(compgen -W "${opt_credentials}" -- ${cur}) )
                return 0;
        fi
        if [[ ${prev} == 'db' ]]; then
                COMPREPLY=( $(compgen -W "${opt_dbs}" -- ${cur}) )
                return 0;
        fi
        
        COMPREPLY=( $(compgen -W "${opts}" -- ${cur}) )
        return 0;
}
complete -F _aci aci
