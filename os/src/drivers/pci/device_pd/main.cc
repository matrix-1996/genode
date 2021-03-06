/*
 * \brief  Pci device protection domain service for pci_drv 
 * \author Alexander Boettcher
 * \date   2013-02-10
 */

/*
 * Copyright (C) 2013-2013 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#include <os/static_root.h>
#include <base/printf.h>
#include <base/sleep.h>

#include <cap_session/connection.h>
#include <dataspace/client.h>
#include <pd_session/client.h>

#include <util/touch.h>

#include "../pci_device_pd_ipc.h"


void Pci::Device_pd_component::attach_dma_mem(Genode::Ram_dataspace_capability ds_cap)
{
	using namespace Genode;

	Dataspace_client ds_client(ds_cap);

	addr_t page = env()->rm_session()->attach_at(ds_cap, ds_client.phys_addr());
	/* sanity check */
	if (page != ds_client.phys_addr())
		throw Rm_session::Region_conflict();

	/* trigger mapping of whole memory area */
	for (size_t rounds = (ds_client.size() + 1) / 4096; rounds;
	     page += 4096, rounds --)
		touch_read(reinterpret_cast<unsigned char *>(page));
}

void Pci::Device_pd_component::assign_pci(Genode::Io_mem_dataspace_capability io_mem_cap)
{
	using namespace Genode;

	Dataspace_client ds_client(io_mem_cap);

	addr_t page = env()->rm_session()->attach(io_mem_cap);
	/* sanity check */
	if (!page)
		throw Rm_session::Region_conflict();

	/* trigger mapping of whole memory area */
	touch_read(reinterpret_cast<unsigned char *>(page));

	/* try to assign pci device to this protection domain */
	if (!env()->pd_session()->assign_pci(page))
		PERR("assignment of PCI device failed");

	/* we don't need the mapping anymore */
	env()->rm_session()->detach(page);
}

int main(int argc, char **argv)
{
	using namespace Genode;

	Genode::printf("PCI device pd starting ...\n");

	/*
	 * Initialize server entry point
	 */
	enum {
		STACK_SIZE       = 4096
	};

	static Cap_connection cap;
	static Rpc_entrypoint ep(&cap, STACK_SIZE, "pci_device_pd_ep");

	static Pci::Device_pd_component pci_device_component;

	/*
	 * Attach input root interface to the entry point
	 */
	static Static_root<Pci::Device_pd> root(ep.manage(&pci_device_component));

	env()->parent()->announce(ep.manage(&root));

	printf("PCI device pd started\n");

	Genode::sleep_forever();
	return 0;
}
