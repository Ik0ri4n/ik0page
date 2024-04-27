<script lang="ts">
	import { DotsVerticalIcon, XIcon } from '@rgossiaux/svelte-heroicons/outline';
	import { createDialog, melt } from '@melt-ui/svelte';
	import Navigation from './Navigation.svelte';

	const {
		elements: { trigger, portalled, overlay, content, title, description, close },
		states: { open }
	} = createDialog();
</script>

<header class="border-b border-gray-300 dark:border-gray-700">
	<div class="mx-auto max-w-7xl">
		<div class="relative flex justify-between px-4 pb-2 pt-4">
			<a class="font-mono text-2xl font-medium text-gray-900 dark:text-gray-50" href="/">
				<span class="text-green-700 dark:text-green-400">Ik0</span>blog
			</a>
			<Navigation
				class="hidden space-x-8 text-lg font-medium text-gray-900 lg:flex dark:text-gray-50"
			/>

			<button
				class="flex h-8 w-8 items-center justify-center text-gray-500 hover:text-gray-600 lg:hidden dark:text-gray-400 dark:hover:text-gray-300"
				use:melt={$trigger}
			>
				<span class="sr-only">Open navigation</span>
				<DotsVerticalIcon class="h-6 w-6" />
			</button>
		</div>
		{#if open}
			<div class="fixed z-50 lg:hidden" use:melt={$portalled}>
				<div
					class="fixed inset-0 bg-gray-900/20 backdrop-blur-sm dark:bg-gray-900/80"
					use:melt={$overlay}
				/>
				<div use:melt={$content}>
					<h2 class="sr-only" use:melt={$title}>Navigation</h2>
					<p class="sr-only" use:melt={$description}>Main navigation menu</p>
					<div
						class="fixed right-4 top-4 w-full max-w-xs rounded-lg bg-gray-50 p-6 shadow-lg dark:bg-gray-800"
						use:melt={$close}
					>
						<button
							class="absolute right-4 top-4 flex h-8 w-8 items-center justify-center text-gray-500 hover:text-gray-600 dark:text-gray-400 dark:hover:text-gray-300"
						>
							<span class="sr-only">Close navigation</span>
							<XIcon class="h-6 w-6" />
						</button>
						<Navigation class="space-y-4 text-lg font-medium text-gray-900 dark:text-gray-50" />
					</div>
				</div>
			</div>
		{/if}
	</div>
</header>
