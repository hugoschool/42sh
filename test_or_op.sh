#!/bin/bash
# Script de test pour l'opérateur || dans 42sh
# À exécuter comme: ./test-or-operator.sh ./42sh

SHELL=$1
TCSH=$(which tcsh)
SUCCESS="\033[32mSUCCESS\033[0m"
FAILURE="\033[31mFAILURE\033[0m"
TEST_COUNT=0
PASS_COUNT=0

# Fonction pour comparer les sorties et codes de retour
compare_outputs() {
    local test_name="$1"
    local command="$2"
    
    TEST_COUNT=$((TEST_COUNT + 1))
    
    # Exécuter dans tcsh (référence)
    echo "$command" | $TCSH > tcsh_out 2> tcsh_err
    TCSH_EXIT=$?
    
    # Exécuter dans 42sh (votre shell)
    echo "$command" | $SHELL > shell_out 2> shell_err
    SHELL_EXIT=$?
    
    # Comparer les codes de sortie
    if [ $TCSH_EXIT -eq $SHELL_EXIT ]; then
        echo -e "Test $TEST_COUNT: $test_name - Exit code: $SUCCESS"
        PASS_COUNT=$((PASS_COUNT + 1))
    else
        echo -e "Test $TEST_COUNT: $test_name - Exit code: $FAILURE"
        echo "  TCSH exit: $TCSH_EXIT"
        echo "  42SH exit: $SHELL_EXIT"
    fi
    
    # Nettoyage
    rm -f tcsh_out tcsh_err shell_out shell_err
}

# Test 1: Commande simple avec OR
compare_outputs "Commande réussie || commande non exécutée" "echo success || echo failure"

# Test 2: Première commande échoue, deuxième s'exécute
compare_outputs "Commande échouée || commande exécutée" "false || echo fallback executed"

# Test 3: Les deux commandes échouent
compare_outputs "Deux commandes échouées" "false || false"

# Test 4: OR avec codes d'erreur variés
compare_outputs "Codes d'erreur variés" "exit 1 || exit 2"

# Test 5: Multiple ORs en chaîne
compare_outputs "Multiple ORs" "false || false || echo last_resort || echo never_shown"

# Test 6: OR avec des commandes complexes
compare_outputs "Commandes complexes" "grep 'pattern' nonexistent_file || echo 'File not found'"

# Test 7: OR avec redirection
compare_outputs "OR avec redirection" "cat nonexistent_file > /dev/null || echo 'Redirect failed'"

# Test 8: OR avec des variables
compare_outputs "OR avec variables" "VAR=test; [ \$VAR = 'wrong' ] || echo 'Variable test failed'"

# Test 9: OR dans une sous-shell
compare_outputs "OR dans sous-shell" "( false || echo 'In subshell' )"

# Test 10: OR combiné avec AND
compare_outputs "OR et AND combinés" "false && echo 'not shown' || echo 'fallback'"

# Test 11: OR avec backquotes
compare_outputs "OR avec backquotes" "echo `false || echo 'inside backticks'`"

# Test 12: OR avec commande inexistante
compare_outputs "OR avec commande inexistante" "nonexistentcommand || echo 'Command not found'"

# Test 13: OR suivi d'une assignation
compare_outputs "OR avec assignation" "false || VAR=test; echo \$VAR"

# Test 14: OR entre deux pipes
compare_outputs "OR entre pipes" "false | echo A || echo B | grep B"

# Test 15: OR avec globbing
compare_outputs "OR avec globbing" "ls /tmp/definitely_not_exists_* || echo 'No match'"

# Résumé des tests
echo "----------------------------------------"
echo "Résumé: $PASS_COUNT/$TEST_COUNT tests réussis"
if [ $PASS_COUNT -eq $TEST_COUNT ]; then
    echo -e "Statut global: $SUCCESS"
else
    echo -e "Statut global: $FAILURE"
fi