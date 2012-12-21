#!/bin/bash
# ==========================
# xsede_setup.sh
#
# OVERVIEW
# This script provides an easy way to automatically configure SSH keys for
# remote computing resources provided through XSEDE.  Simply run it as:
#
#     ./xsede_setup.sh
#
# It will run some initial setup (including starting ssh-agent and getting
# a Kerberos ticket for gsissh via myproxy-logon) and then sequentially
# process each machine in the following list, setting up a local SSH alias,
# and optionally copying over a public key for automatic login and installing
# an SSH alias back to Swarthmore on the remote machine.
#
# Note that this assumes that your XSEDE username is the same as your Swat
# CS username; if this is not the case, set 'XSEDE_USER' below.
#
# This script is based in on the instructions from the following pages:
#  - http://svn.cs.swarthmore.edu/gis_mpi/wiki/XsedeAccount
#  - http://svn.cs.swarthmore.edu/gis_mpi/wiki/XsedeSSH
#
# Nick Felt, 11 March 2012.

# ----------------------
# LIST OF XSEDE MACHINES
# Each entry in this list should either be a comment (a line starting with #)
# or a line of the following form:
#
# <short name> <full domain name> <y|n>
#
# Short name is the name of the SSH config file host alias to create; the
# full domain name is the actual hostname of the server, and the last y/n
# value controls whether or not to run the remote part of the script for
# that machine (y is do run it; n is do not).
XSEDE_MACHINES=$(cat <<'EOF'
# Blacklight requires a special procedure for installing SSH keys; see
# http://www.psc.edu/general/net/ssh/sshkeys.php for more detail.
blacklight blacklight.psc.teragrid.org n
condor tg-condor.purdue.teragrid.org y
forge login-forge.ncsa.xsede.org y
lonestar lonestar.tacc.teragrid.org y
longhorn tg-login.longhorn.tacc.teragrid.org y
# Nautilus requires a special OTP-based procedure for logging in and doesn't
# even support gsissh - see https://www.xsede.org/web/guest/nics-nautilus .
nautilus login.nautilus.nics.xsede.org n
ranger tg-login.ranger.tacc.teragrid.org y
# Spur shares ranger's filesystems, so we can skip its remote setup.
spur tg-login.spur.tacc.teragrid.org n
# Steele shares condor's filesystems, so we can skip its remote setup.
steele tg-steele.purdue.teragrid.org n
trestles trestles.sdsc.edu y
EOF
)

# ---------------
# USER PARAMETERS
# The name of the SSH host alias to create on remote machines back to Swat.
SWAT_ALIAS=swat
# Where that alias should point (presumably the CS lab domain name).
SWAT_FULL_HOSTNAME=lab.cs.swarthmore.edu
# What public key file to use in ~/.ssh (change if you use DSA).
PUBLIC_KEY=id_rsa.pub
# What your Swat CS username is (presumably the one you are using now...).
SWAT_USER=$(whoami)
# What your XSEDE username is (if not $SWAT_USER, specify this manually).
XSEDE_USER=shwang

# ----------------
# OTHER PARAMETERS
SSH_DIR=~/.ssh
SSH_CONFIG="$SSH_DIR"/config
SSH_HOST_TEMPLATE=$(cat <<'EOF'
Host %s
     HostName %s
     User %s\n
EOF
)
SSH_AUTH_KEYS="$SSH_DIR"/authorized_keys
SSH_PUBLIC_KEY="$SSH_DIR/$PUBLIC_KEY"

SCRIPT_PATH="$0"
REMOTE_SCRIPT_NAME=./$(basename "$SCRIPT_PATH")

# Borrowed from guide on the CS website at:
# http://svn.cs.swarthmore.edu/gis_mpi/wiki/XsedeAccount
GLOBUS_LOCATION=/usr/local/stow/globus
MYPROXY_SERVER=myproxy.teragrid.org
MYPROXY_SERVER_PORT=7514

# --------------
# BASH FUNCTIONS

# Runs the script in remote mode; sets up local copy of SSH public key
# and SSH host alias for Swarthmore CS.
run_remote() {
    echo "Running script remotely on $(hostname)..."
    SWAT_USER="$1"
    mkdir -p "$SSH_DIR"
    printf "\n##\n## SWAT MACHINE CONFIGURATION\n##\n" >> "$SSH_CONFIG"
    printf "$SSH_HOST_TEMPLATE" "$SWAT_ALIAS $SWAT_FULL_HOSTNAME" \
	"$SWAT_FULL_HOSTNAME" "$SWAT_USER" >> "$SSH_CONFIG"
    cat "$PUBLIC_KEY.tmp" >> "$SSH_AUTH_KEYS"
    rm -f "$PUBLIC_KEY.tmp"
}

# Runs the script in local mode; runs initial setup steps and then iterates
# over machines list and does setup for each machine.
run_local() {
    echo "Running XSEDE account setup script."
    echo
    echo "Below, ssh-agent will prompt for your CS public key passphrase:"
    run_ssh_agent || exit 1
    echo
    echo "Below, myproxy-logon will prompt for your XSEDE account password:"
    run_myproxy_logon || exit 1
    echo
    printf "\n##\n## XSEDE MACHINE CONFIGURATION\n##\n" >> "$SSH_CONFIG"
    for machine in $(echo "$XSEDE_MACHINES" | tr ' ' '_'); do
	comment="#"
	[[ "$machine" = $comment* ]] && continue
	machine=$(echo $machine | tr '_' ' ')
	read -r name full_name do_remote <<< "$machine"
	setup_machine_local "$name" "$full_name"
	if [[ "$do_remote" = "y" ]]; then 
	    setup_machine_remote "$name" "$full_name"
	    test_machine_setup "$name" "$full_name"
	fi
	echo
    done
}

# Runs ssh-agent so that we can use password-less ssh later.
run_ssh_agent() {
    eval $(ssh-agent -s)
    ssh-add
}

# Runs myproxy-logon so that we can use gsissh/gsiscp later.
run_myproxy_logon() {
    export GLOBUS_LOCATION MYPROXY_SERVER MYPROXY_SERVER_PORT
    $GLOBUS_LOCATION/etc/globus-user-env.sh
    myproxy-logon -T -t 1 -l shwang
}

# Sets up the local side of configuration for a given XSEDE resource, i.e.
# makes an SSH host alias in ~/.ssh/config.
setup_machine_local() {
    read -r name full_name <<< "$@"
    echo "Setting up $name ($full_name)."
    printf "$SSH_HOST_TEMPLATE" "$name $full_name" "$full_name" \
	"$XSEDE_USER" >> "$SSH_CONFIG"
}

# Sets up the remote side of configuration for a given XSEDE resource, i.e.
# copies your SSH public key and this script to the remote resource using
# gsiscp and then uses gsissh to run this script in remote mode.
setup_machine_remote() {
    read -r name full_name <<< "$@"
    echo "Copying files to remote account:"
    gsiscp "$SCRIPT_PATH" "$full_name:$REMOTE_SCRIPT_NAME"
    gsiscp "$SSH_PUBLIC_KEY" "$full_name:$PUBLIC_KEY.tmp"
    gsissh "$full_name" "$REMOTE_SCRIPT_NAME --remote $SWAT_USER && \
                         rm -f $REMOTE_SCRIPT_NAME"
}

# Tests to ensure that the setup is working, by attempting to use password-
# free SSH with the local host alias to verify that login to the remote
# resource works as expected.  Prints success or failure.
test_machine_setup() {
    read -r name full_name <<< "$@"
    ssh -o StrictHostKeyChecking=no -o PasswordAuthentication=no -n \
	-l "$XSEDE_USER" "$name" \
	'echo -n "Setup succeeded on "; hostname' || \
	echo "Setup failed on $full_name"
}

# "main()" - runs the script in local mode by default, or in remote mode
# if the script was passed the flag --remote.
if [[ "$1" = "--remote" ]] ; then
    shift 1
    run_remote "$@"
else
    run_local
fi
