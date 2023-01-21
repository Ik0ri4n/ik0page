<script lang="ts">
	import { DotsVerticalIcon, XIcon } from '@rgossiaux/svelte-heroicons/outline';
	import {
		Dialog,
		DialogOverlay,
		DialogTitle,
		DialogDescription
	} from '@rgossiaux/svelte-headlessui';
	import Navigation from './Navigation.svelte';
	let isOpen = false;
</script>

<header class="border-b border-gray-300 dark:border-gray-700">
	<div class="mx-auto max-w-7xl">
		<div class="relative flex justify-between px-4 pt-4 pb-2">
			<a class="font-mono text-2xl font-medium text-gray-900 dark:text-gray-50" href="/">
				<span class="text-green-700 dark:text-green-400">Ik0</span>blog
			</a>
			<Navigation class="hidden lg:flex space-x-8 text-lg font-medium text-gray-900 dark:text-gray-50" />

			<button
				class="flex lg:hidden h-8 w-8 items-center justify-center text-gray-500 hover:text-gray-600 dark:text-gray-400 dark:hover:text-gray-300"
				on:click={() => (isOpen = true)}
			>
				<span class="sr-only">Open navigation</span>
				<DotsVerticalIcon class="h-6 w-6" />
			</button>
		</div>
		<Dialog class="fixed z-50 lg:hidden" open={isOpen} on:close={() => (isOpen = false)}>
			<DialogTitle class="sr-only">Navigation</DialogTitle>
			<DialogDescription class="sr-only">Main navigation menu</DialogDescription>
			<DialogOverlay class="fixed inset-0 bg-gray-900/20 backdrop-blur-sm dark:bg-gray-900/80" />
			<div
				class="fixed top-4 right-4 w-full max-w-xs rounded-lg bg-gray-50 p-6 shadow-lg dark:bg-gray-800"
			>
				<button
					class="absolute top-4 right-4 flex h-8 w-8 items-center justify-center text-gray-500 hover:text-gray-600 dark:text-gray-400 dark:hover:text-gray-300"
					on:click={() => (isOpen = false)}
				>
					<span class="sr-only">Close navigation</span>
					<XIcon class="h-6 w-6" />
				</button>
				<Navigation class="space-y-4 text-lg font-medium text-gray-900 dark:text-gray-50"/>
			</div>
		</Dialog>
	</div>
</header>
